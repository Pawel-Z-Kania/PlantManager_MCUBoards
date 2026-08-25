#!/usr/bin/env python3
"""Orchestrator testow bring-up dla PlantManager.

Wgrywa kolejno srodowiska testowe (test0..test3) na kazda plytke, czyta wynik
przez port szeregowy (znaczniki ===TEST_SUITE_START/END=== z self_test.h) i
zatrzymuje sie na pierwszym FAIL. Na koncu wgrywa test4_nrf_link na obie
plytki jednoczesnie i liczy wspolczynnik dostarczonych pakietow.

Wymaga: pip install pyserial
Uruchomienie (z katalogu PlantManager_CentralControlBoard): python tools/run_bringup.py [--uno] [--only detector|central|both]
"""
import argparse
import re
import subprocess
import sys
import time
from dataclasses import dataclass, field
from typing import List

import serial

PLATFORMIO = r"C:/Users/euter/.platformio/penv/Scripts/platformio.exe"

STAGE_TIMEOUT_S = 15
LINK_DURATION_S = 15
LINK_MIN_SUCCESS_RATE = 0.95

SUITE_START_RE = re.compile(r"===TEST_SUITE_START:(\S+)===")
SUITE_END_RE = re.compile(r"===TEST_SUITE_END:RESULT=(PASS|FAIL)===")
FAIL_LINE_RE = re.compile(r"^\[FAIL\] (.+)$")
SENT_RE = re.compile(r"Wysy.am pakiet nr: (\d+)")
RECEIVED_RE = re.compile(r"Odebrano pakiet #(\d+)")


@dataclass
class Board:
    name: str
    project_dir: str
    port: str
    baud: int
    stage_envs: List[str]
    link_env: str


@dataclass
class StageResult:
    env: str
    passed: bool
    fail_reasons: List[str] = field(default_factory=list)
    timed_out: bool = False


def upload(board: Board, env: str) -> None:
    print(f"  -> budowanie i wgrywanie {board.name}/{env} ...")
    result = subprocess.run(
        [PLATFORMIO, "run", "-e", env, "-t", "upload"],
        cwd=board.project_dir,
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        print(result.stdout[-2000:])
        print(result.stderr[-2000:])
        raise RuntimeError(f"Upload {board.name}/{env} nie powiodl sie (kod {result.returncode})")


def read_stage_result(board: Board, timeout_s: float = STAGE_TIMEOUT_S) -> StageResult:
    deadline = time.time() + timeout_s
    fail_reasons: List[str] = []
    env_name = "?"

    with serial.Serial(board.port, board.baud, timeout=1) as ser:
        while time.time() < deadline:
            line = ser.readline().decode(errors="replace").strip()
            if not line:
                continue

            start_match = SUITE_START_RE.search(line)
            if start_match:
                env_name = start_match.group(1)

            fail_match = FAIL_LINE_RE.match(line)
            if fail_match:
                fail_reasons.append(fail_match.group(1))

            end_match = SUITE_END_RE.search(line)
            if end_match:
                passed = end_match.group(1) == "PASS"
                return StageResult(env=env_name, passed=passed, fail_reasons=fail_reasons)

    return StageResult(env=env_name, passed=False, fail_reasons=fail_reasons, timed_out=True)


def run_single_board_stages(board: Board) -> bool:
    print(f"\n=== {board.name} ({board.port}) ===")
    for env in board.stage_envs:
        upload(board, env)
        result = read_stage_result(board)
        status = "PASS" if result.passed else ("TIMEOUT" if result.timed_out else "FAIL")
        print(f"  [{status}] {env}")
        for reason in result.fail_reasons:
            print(f"      - {reason}")
        if not result.passed:
            return False
    return True


def run_link_test(detector: Board, central: Board) -> bool:
    print(f"\n=== Test integracyjny: {detector.link_env} ===")
    upload(detector, detector.link_env)
    upload(central, central.link_env)

    max_sent = 0
    max_received = 0
    deadline = time.time() + LINK_DURATION_S

    with serial.Serial(detector.port, detector.baud, timeout=1) as tx_ser, \
            serial.Serial(central.port, central.baud, timeout=1) as rx_ser:
        while time.time() < deadline:
            tx_line = tx_ser.readline().decode(errors="replace").strip()
            if tx_line:
                m = SENT_RE.search(tx_line)
                if m:
                    max_sent = max(max_sent, int(m.group(1)))

            rx_line = rx_ser.readline().decode(errors="replace").strip()
            if rx_line:
                m = RECEIVED_RE.search(rx_line)
                if m:
                    max_received = max(max_received, int(m.group(1)))

    if max_sent == 0:
        print("  [FAIL] Nadajnik nie wyslal zadnego pakietu w oknie czasowym")
        return False

    success_rate = max_received / max_sent
    print(f"  Wyslano: {max_sent}, Odebrano: {max_received}, Skutecznosc: {success_rate:.0%}")

    passed = success_rate >= LINK_MIN_SUCCESS_RATE
    print(f"  [{'PASS' if passed else 'FAIL'}] {detector.link_env} (prog {LINK_MIN_SUCCESS_RATE:.0%})")
    return passed


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--uno", action="store_true", help="Detector to plytka UNO (COM3) zamiast Pro Mini (COM5)")
    parser.add_argument(
        "--only",
        choices=["detector", "central", "both"],
        default="both",
        help="Testuj tylko jedna plytke (bez etapow drugiej i bez testu integracyjnego) lub obie (domyslnie)",
    )
    args = parser.parse_args()

    uno_suffix = "_uno" if args.uno else ""
    detector = Board(
        name="Detector",
        project_dir="../PlantManager_DetectorBoard",
        port="COM3" if args.uno else "COM5",
        baud=9600,
        stage_envs=[f"test0_selftest{uno_suffix}", f"test1_serial{uno_suffix}",
                    f"test2_sensor{uno_suffix}", f"test3_nrf_chip{uno_suffix}"],
        link_env=f"test4_nrf_link{uno_suffix}",
    )
    central = Board(
        name="Central",
        project_dir=".",
        port="COM6",
        baud=115200,
        stage_envs=["test0_selftest", "test1_serial", "test2_wifi", "test3_nrf_chip"],
        link_env="test4_nrf_link",
    )

    if args.only in ("detector", "both"):
        if not run_single_board_stages(detector):
            print("\nWYNIK: FAIL - zatrzymano na Detector, nie testowano dalej")
            return 1

    if args.only in ("central", "both"):
        if not run_single_board_stages(central):
            print("\nWYNIK: FAIL - zatrzymano na Central, nie testowano dalej")
            return 1

    if args.only != "both":
        print(f"\nWYNIK: TESTY {args.only.upper()} PRZESZLY (test integracyjny pominiety - uzyto --only)")
        return 0

    if not run_link_test(detector, central):
        print("\nWYNIK: FAIL - test integracyjny NRF nie przeszedl")
        return 1

    print("\nWYNIK: WSZYSTKIE TESTY PRZESZLY")
    return 0


if __name__ == "__main__":
    sys.exit(main())

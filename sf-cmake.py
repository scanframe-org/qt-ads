#!/usr/bin/env python3
# coding=utf-8
"""
CMake Library Cloner

Clones the cmake-lib repository into the cmake/lib directory at a specified tag.
"""

from __future__ import annotations
import argparse
import subprocess
import sys
from pathlib import Path

# Default repository URL and tag
REPO_URL = "https://github.com/scanframe-org/sf-cmake.git"
DEFAULT_TAG = "latest"
TARGET_DIR = Path("cmake/lib")


# Parser instance for reuse
parser = argparse.ArgumentParser(
	description="Clone the cmake-lib repository at a specific tag."
)
parser.add_argument(
	"-c",
	"--clone",
	action="store_true",
	help="Clone the repository. Without this flag, help is shown.",
)
parser.add_argument(
	"-t",
	"--tag",
	default=DEFAULT_TAG,
	help=f"Repository tag to clone (default: {DEFAULT_TAG}).",
)


# Parse command-line arguments
def parse_args() -> argparse.Namespace:
	return parser.parse_args()


# Main entry point
def main() -> int:
	args = parse_args()

	# Show help if --clone flag is not provided
	if not args.clone:
		parser.print_help()
		return 0

	# Check if target directory already exists
	if TARGET_DIR.exists():
		print(f"Error: Directory '{TARGET_DIR}' already exists. Aborting.", file=sys.stderr)
		return 1

	# Create parent directory if needed
	TARGET_DIR.parent.mkdir(parents=True, exist_ok=True)

	# Clone the repository at the specified tag
	print(f"Cloning {REPO_URL} at tag {args.tag} into {TARGET_DIR}...")
	result = subprocess.run(
		["git", "clone", "--branch", args.tag, "--single-branch", REPO_URL, str(TARGET_DIR)],
		capture_output=True,
		text=True,
	)

	if result.returncode != 0:
		print(f"Error: git clone failed.\n{result.stderr}", file=sys.stderr)
		return result.returncode

	print(f"Successfully cloned cmake-lib at tag {args.tag}.")
	return 0


if __name__ == "__main__":
	sys.exit(main())

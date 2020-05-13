#!/usr/bin/env python3

# This script expects tags to be in the form of v{MAJOR}.{MINOR}.{PATCH}
# EX: v2.11.2, v3.1.4


import sys
import subprocess
import argparse


def run_command(cmd):
    ret = err = ""
    try:
        ret = subprocess.check_output(cmd.split(' '), stderr=subprocess.STDOUT).decode("utf-8").strip()
    except subprocess.CalledProcessError as e:
        err = (e.returncode, e.cmd, e.output.decode("utf-8"))
    return ret, err

def get_tag():
    tag, err = run_command("git describe --tags --match=v*")
    return tag.split('-')[0] if not err else ""

def get_commit():
    commit, err = run_command("git rev-parse --short=8 HEAD")
    commit = commit[:8]
    return commit if not err else ""

def get_major():
    tag = get_tag()
    return tag[1:].split('.')[0] if tag else ""

def get_minor():
    tag = get_tag()
    return tag[1:].split('.')[1] if tag else ""

def get_patch():
    tag = get_tag()
    try:
        patch = tag[1:].split('.')[2] if tag else ""
    except IndexError:
        # Pre-semantic versioning tag
        patch = "0"
    return patch

def get_develop():
    _, err = run_command("git describe --exact-match --tags")
    return "develop" if err else ""

def get_dirty():
    upstream, _ = run_command("git rev-parse @{u}")
    head, _ = run_command("git rev-parse HEAD")
    unpushed_commits = (upstream != head)
    modified_files, _ = run_command("git status --porcelain")
    return "dirty" if unpushed_commits or modified_files else ""

def get_next_major():
    return int(get_major()) + 1

def get_next_minor():
    return int(get_minor()) + 1

def get_next_patch():
    return int(get_patch()) + 1

def get_full_version():
    major = get_major()
    minor = get_minor()
    patch = get_patch()
    develop = get_develop()
    dirty = get_dirty()
    commit = get_commit()

    version = ".".join([major, minor, patch])
    prerelease = "-".join(list(filter(None, [develop, dirty])))
    prerelease = "-" + prerelease if prerelease else ''

    full = version + prerelease + "+" + commit
    return full

def get_all():
    return {
        "tag": get_tag(),
        "commit": get_commit(),
        "major": get_major(),
        "minor": get_minor(),
        "patch": get_patch(),
        "develop": get_develop(),
        "dirty": get_dirty(),
        "next_major": get_next_major(),
        "next_minor": get_next_minor(),
        "next_patch": get_next_patch(),
        "full": get_full_version()
    }

def parse_cli_args():
    desc = "supplies version information based on git tags"
    parser = argparse.ArgumentParser(description = desc)
    parser.add_argument("-f", "--full", help="complete version string", action="store_true")
    parser.add_argument("-t", "--tag", help="current version tag", action="store_true")
    parser.add_argument("-c", "--commit", help="current commit hash", action="store_true")
    parser.add_argument("-m", "--major", help="current major version number", action="store_true")
    parser.add_argument("-n", "--minor", help="current minor version number", action="store_true")
    parser.add_argument("-p", "--patch", help="current patch version number", action="store_true")
    parser.add_argument("-d", "--develop", help="check if built off a commit without a tag", action="store_true")
    parser.add_argument("-D", "--dirty", help="check if built with unpushed changes", action="store_true")
    parser.add_argument("-M", "--next_major", help="gets next major version number", action="store_true")
    parser.add_argument("-N", "--next_minor", help="get next minor version number", action="store_true")
    parser.add_argument("-P", "--next_patch", help="get next patch version number", action="store_true")
    parser.add_argument("-a", "--all", help="returns a dictionary of all the arguments", action="store_true")


    args = parser.parse_args()
    answer = ""

    if args.tag:          answer = get_tag()
    elif args.commit:     answer = get_commit()
    elif args.major:      answer = get_major()
    elif args.minor:      answer = get_minor()
    elif args.patch:      answer = get_patch()
    elif args.develop:    answer = get_develop()
    elif args.dirty:      answer = get_dirty()
    elif args.next_major: answer = get_next_major()
    elif args.next_minor: answer = get_next_minor()
    elif args.next_patch: answer = get_next_patch()
    elif args.full:       answer = get_full_version()
    elif args.all:        answer = get_all()
    else:
        parser.print_help(sys.stderr)
        sys.exit(1)

    print(answer)

if __name__ == "__main__":
    parse_cli_args()


import argparse
from .cpio import pack_cpio, unpack_cpio
import logging


def cli():
    logging.basicConfig(level=logging.DEBUG)

    parser = argparse.ArgumentParser(
        prog="pwnix",
        description="Linux Pwning Toolkit",
    )

    opt_group = parser.add_mutually_exclusive_group(required=True)
    opt_group.add_argument("-c", "--cpio", help="cpio command")
    opt_group.add_argument("-r", "--rootfs", help="rootfs path")

    parser.add_argument("-o", "--output", help="output file")

    args = parser.parse_args()

    if args.cpio:
        unpack_cpio(args.cpio, args.output)
    elif args.rootfs:
        pack_cpio(args.rootfs, args.output)

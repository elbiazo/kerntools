import argparse
from .packers import pack_cpio, unpack_cpio, extract_bzimage
import logging


def cli():
    logging.basicConfig(level=logging.DEBUG)

    parser = argparse.ArgumentParser(
        prog="kerntools",
        description="Linux Kernel Pwning Toolkit",
    )

    opt_group = parser.add_mutually_exclusive_group(required=True)
    opt_group.add_argument("-c", "--cpio", help="cpio command")
    opt_group.add_argument("-r", "--rootfs", help="rootfs path")
    opt_group.add_argument("-z", "--bzimage", help="bzImage or vmlinuz path")
    opt_group.add_argument(
        "-l", "--lib", action="store_true", help="returns libpn header path"
    )

    parser.add_argument("-o", "--output", help="output file")

    args = parser.parse_args()

    if args.cpio:
        unpack_cpio(args.cpio, args.output)
    elif args.rootfs:
        pack_cpio(args.rootfs, args.output)
    elif args.bzimage:
        extract_bzimage(args.bzimage, args.output)
    elif args.lib:
        import os
        from pathlib import Path

        dir_path = os.path.dirname(os.path.realpath(__file__))
        libkt = Path(dir_path).parent / "libkt"
        # print(f"libkt :{libkt}")
        cmake = rf"""include_directories({libkt}/include)
add_subdirectory({libkt} subproject/libkt)"""
        print("cmake:")
        print(cmake)
        print("\nMake sure to link libkt by:")
        print("target_link_libraries(<prog_name> PRIVATE kt)")

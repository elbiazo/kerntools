from pathlib import Path
import os
from subprocess import Popen, PIPE, STDOUT, DEVNULL
import logging


def is_path_empty(path):
    return not any(path.iterdir())


def unpack_cpio(cpio: str, rootfs: str):
    cpio_path = Path(cpio)
    rootfs_path = cpio_path.parent.absolute() / "root"

    if rootfs:
        rootfs_path = Path(rootfs)

    # If rootfs doesn't exist, create it
    if not rootfs_path.exists():
        rootfs_path.mkdir(parents=True)

    logging.info(msg=f"Unpacking {str(cpio_path)} to {str(rootfs_path)}")

    # When you change chdir, it chages the absolute path. not sure why. Make sure you restore to org path
    # cpio = str(cpio_path.absolute())
    # org_path = os.getcwd()
    # os.chdir(rootfs_path)
    cmd = f"(cd {rootfs_path.absolute()} && cpio -idv < {cpio_path.absolute()})"
    out, err = Popen(cmd, shell=True, stdout=DEVNULL, stderr=PIPE).communicate()
    # os.chdir(org_path)


def pack_cpio(rootfs: str, cpio: str):
    rootfs_path = Path(rootfs)
    default_output_path = rootfs_path.parent.absolute() / "rootfs_updated.cpio"
    cpio_path = default_output_path

    if cpio:
        cpio_path = Path(cpio)

    # check if cpio extention is .cpio
    cpio_ext = cpio_path.suffix
    if cpio_ext != ".cpio":
        logging.warn(
            msg="cpio extention is not .cpio. Changing to default (rootfs_updated.cpio)"
        )
        cpio_path = default_output_path

    cmd = f"(cd {rootfs_path.absolute()} && find . -print0 | cpio -o --format=newc --null --owner=root > {cpio_path})"
    # # Not sure why cpio sends output to stderr
    _, out = Popen(cmd, shell=True, stdout=DEVNULL, stderr=PIPE).communicate()
    logging.info(msg=f"Created {str(cpio_path)}. {out}")

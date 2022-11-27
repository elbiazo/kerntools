# WIP: This is used in snippet.
printk = bv.get_symbols_by_name("printk")[0]
if printk:
    for ref in bv.get_callers(printk.address):
        if isinstance(ref.hlil, HighLevelILCall):
            if len(ref.hlil.params) > 0:
                if isinstance(ref.hlil.params[0], HighLevelILConst):
                    str_addr = ref.hlil.params[0].value.value
                    ref_addr = ref.address
                    print(f"{ref_addr:#x} @ {str_addr:#x}")
                    pstr = str(bv.get_ascii_string_at(str_addr))[2:]
                    bv.set_comment_at(ref_addr, pstr)
                else:
                    log.warn(f"Couldn't get string ref for {ref.address:#x}")
else:
    log.info("Can't find printk")

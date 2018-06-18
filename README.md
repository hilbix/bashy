# LVM helpers

Some operations on LVM are not really straight forward.

For example you can defrag your LVM with pvmove.  On the fly.
Or you can poke holes in a PV to skip over defective areas.

Such tools will show up here when I find the time to do them.

## `sudo lvminfo [PVs]`

Dumps `pvdisplay -m` as a table, such that you can easily postprocess it.

If `PVs` are given, only the given PVs are dumped in an extended format (nearly all info is included, which is a bit repetitive).


### Examples

- `sudo lvminfo | sort -n` sorts the information on the LVs, such that you can see where all the fragments go.  This is intentional (do not miss the `-n` to `sort`).


# Foenix Toolbox: Binary

This folder contains the binary library files for the Toolbox as a static link library.
Library files will be named in the format:
<pre>
toolbox_{model}_{codemodel}_{datamodel}.a
</pre>
where <code>{model}</code> is the name of the target computer, <code>{codemodel}</code> is the
two letter Calypsi code model designator (_e.g._ <code>lc</code> for large, <code>sc</code> for small), and
<code>{datamodel}</code> is the two letter Calypsi data model designator (_e.g._ <code>ld</code>).
For instance:
<pre>
toolbox_fa2560k2_lc_ld.a
</pre>
is the static library for the FA2560K2 compiled to use the large code and data models.

This branch adds support for [RFC4180](https://tools.ietf.org/html/rfc4180.html) type csv files.  The first line of the output file is a comma separated list of field names.  The remaining lines in the file make up the computed prediction data.  In accordance with RFC4180, whitespace is removed.  These files are of use when exporting the results to an external application for analysis and can be parsed directly using Pythons csv modules and pandas etc.

This option is called using the '-c' command line argument.

The mime type for this type of output file is;

    text/csv; header=present





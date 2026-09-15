/* Print machine-friendly AmiNTP diagnostics. */
OPTIONS RESULTS
CALL issue 'PING'
CALL issue 'VERSION'
CALL issue 'STATUS'
CALL issue 'SERVER'
CALL issue 'LASTSYNC'
CALL issue 'LASTERROR'
EXIT 0

issue:
  PARSE ARG command
  DROP RESULT
  ADDRESS AMINTP command
  cmdrc = RC
  SAY 'COMMAND=' || command
  SAY 'RC=' || cmdrc
  IF SYMBOL('RESULT') = 'VAR' THEN SAY 'RESULT=' || RESULT
  ELSE SAY 'RESULT=<unset>'
RETURN

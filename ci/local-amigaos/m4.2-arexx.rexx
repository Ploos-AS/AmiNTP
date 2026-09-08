/* Real RexxMast/AMINTP network probe. No PASS inferred from completion. */
OPTIONS RESULTS
PARSE ARG server
IF server = '' THEN EXIT 20
CALL issue 'QUERY SERVER=' || server
queryrc = cmdrc
/* CLI query/config gates must also have completed before clock mutation. */
IF queryrc = 0 & EXISTS('Q:allow-sync') THEN DO
  ADDRESS COMMAND 'C:Date >Q:arexx-sync-before.txt'
  CALL issue 'SYNC SERVER=' || server || ' NORTC'
  ADDRESS COMMAND 'C:Date >Q:arexx-sync-after.txt'
END
CALL issue 'LASTSYNC'
CALL issue 'LASTERROR'
CALL issue 'QUIT'
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

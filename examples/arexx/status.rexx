/* Display useful AmiNTP resident status. */
OPTIONS RESULTS
CALL show 'VERSION'
CALL show 'STATUS'
CALL show 'SERVER'
CALL show 'LASTSYNC'
CALL show 'LASTERROR'
EXIT 0

show:
  PARSE ARG command
  DROP RESULT
  ADDRESS AMINTP command
  IF SYMBOL('RESULT') = 'VAR' THEN SAY command || ': ' || RESULT
  ELSE SAY command || ': <unset> (RC=' || RC || ')'
RETURN

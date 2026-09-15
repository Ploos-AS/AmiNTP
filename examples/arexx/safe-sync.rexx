/* Query first; only synchronize if the server answers successfully. */
OPTIONS RESULTS
PARSE ARG server
IF server = '' THEN server = 'pool.ntp.org'

ADDRESS AMINTP 'QUERY SERVER=' || server
IF RC ~= 0 THEN DO
  SAY 'AmiNTP QUERY failed; clock left unchanged. RC=' || RC
  ADDRESS AMINTP 'LASTERROR'
  IF RC = 0 THEN SAY RESULT
  EXIT 10
END

ADDRESS AMINTP 'SYNC SERVER=' || server
IF RC ~= 0 THEN DO
  SAY 'AmiNTP SYNC failed, RC=' || RC
  ADDRESS AMINTP 'LASTERROR'
  IF RC = 0 THEN SAY RESULT
  EXIT 10
END
SAY RESULT
EXIT 0

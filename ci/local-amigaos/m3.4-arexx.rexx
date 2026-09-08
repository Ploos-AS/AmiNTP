/* M3.4 local AmigaOS ARexx qualification for AmiNTP.
 * Run with RexxMast active and AmiNTP RESIDENT already started.
 */

OPTIONS RESULTS

failures = 0

CALL checkcmd 'PING', 'PONG'
CALL checkcmd 'VERSION', 'AmiNTP 0.3.2-m3.2'
CALL checkcmd 'STATUS', 'OK MODE=RESIDENT PORT=AMINTP'
CALL checkcmd 'LASTSYNC', ''
CALL checkcmd 'LASTERROR', ''
CALL checkcmd 'QUIT', 'OK QUIT'

IF failures = 0 THEN DO
  SAY 'STATUS=PASS'
  SAY 'GATE=M3_4_LOCAL_AMIGAOS_AREXX'
  EXIT 0
END

SAY 'STATUS=FAIL'
SAY 'GATE=M3_4_LOCAL_AMIGAOS_AREXX'
SAY 'FAILURES=' || failures
EXIT 10

checkcmd:
  PARSE ARG command, expected
  ADDRESS AMINTP command
  cmdrc = RC
  cmdresult = RESULT
  SAY 'COMMAND=' || command
  SAY 'RC=' || cmdrc
  SAY 'RESULT=' || cmdresult
  IF cmdrc ~= 0 THEN DO
    failures = failures + 1
    RETURN
  END
  IF expected ~= '' & POS(expected, cmdresult) = 0 THEN DO
    failures = failures + 1
  END
RETURN

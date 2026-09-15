/* Check that the AmiNTP ARexx port is alive. */
OPTIONS RESULTS
ADDRESS AMINTP 'PING'
IF RC ~= 0 THEN DO
  SAY 'AmiNTP did not answer, RC=' || RC
  EXIT 10
END
SAY RESULT
EXIT 0

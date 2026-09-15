/* Startup-friendly synchronization.
 * Failure is reported but does not abort the surrounding startup sequence.
 */
OPTIONS RESULTS
PARSE ARG server
IF server = '' THEN server = 'pool.ntp.org'

ADDRESS AMINTP 'PING'
IF RC ~= 0 THEN DO
  SAY 'AmiNTP: resident ARexx port is not available; skipping startup sync.'
  EXIT 0
END

ADDRESS AMINTP 'SYNC SERVER=' || server
IF RC ~= 0 THEN DO
  SAY 'AmiNTP: startup synchronization failed; continuing startup.'
  ADDRESS AMINTP 'LASTERROR'
  IF RC = 0 THEN SAY 'AmiNTP: ' || RESULT
  EXIT 0
END
SAY 'AmiNTP: clock synchronized.'
EXIT 0

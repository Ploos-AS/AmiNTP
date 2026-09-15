/* Query an NTP server without changing the clock.
 * Usage: RX query.rexx [server]
 */
OPTIONS RESULTS
PARSE ARG server
IF server = '' THEN server = 'pool.ntp.org'
ADDRESS AMINTP 'QUERY SERVER=' || server
IF RC ~= 0 THEN DO
  SAY 'QUERY failed, RC=' || RC
  EXIT 10
END
SAY RESULT
EXIT 0

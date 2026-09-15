/* Synchronize the Amiga clock.
 * Usage: RX sync.rexx [server]
 */
OPTIONS RESULTS
PARSE ARG server
IF server = '' THEN server = 'pool.ntp.org'
ADDRESS AMINTP 'SYNC SERVER=' || server
IF RC ~= 0 THEN DO
  SAY 'SYNC failed, RC=' || RC
  EXIT 10
END
SAY RESULT
EXIT 0

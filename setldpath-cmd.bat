@ECHO OFF
SET PATH=%PATH%;%CD%\lib\win\
SET TMP=PATH
FOR /D %%M IN (modules\*) DO CALL SET PATH=%%%TMP%%%;%CD%\%%M\dist\win\

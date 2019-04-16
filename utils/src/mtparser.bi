' General text parsing routines
' Copyleft 2016 The Mojon Twins

Declare Sub parseCoordinatesString (coordsString as String, coords () As Integer)
Declare Sub parseTokenizeString (inString As String, tokens () As String, ignore As String, break As String)
Declare Function parserFindTokenInTokens (token As String, tokens () As String, modifier As String) As Integer

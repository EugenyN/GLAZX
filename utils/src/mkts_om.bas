' mkts_om v0.1 - Converts graphics
' Copyleft 2017 The Mojon Twins

' fbc mkts_om.bas cmdlineparser.bas mtparser.bas

' Todo: support unmasked sprites

#include "file.bi"
#include "fbpng.bi"
#include "fbgfx.bi"
#include once "crt.bi"

#include "cmdlineparser.bi"
#include "mtparser.bi"

#define RGBA_R( c ) ( CUInt( c ) Shr 16 And 255 )
#define RGBA_G( c ) ( CUInt( c ) Shr  8 And 255 )
#define RGBA_B( c ) ( CUInt( c )        And 255 )
#define RGBA_A( c ) ( CUInt( c ) Shr 24         )

Type RGBType
	r As Integer
	g As Integer
	b As Integer
End Type

Const PLATFORM_ZX = 0
Const PLATFORM_CPC = 1

Dim Shared As Integer silent, flipped, upsideDown, debug

Dim Shared As uByte mainBin (65535)
Dim Shared As uByte auxBin (65535)
Dim Shared As String cPool (255)
Dim Shared As uByte tMaps (255, 255)
Dim Shared As uByte nametable (1535)
Dim Shared As uByte nametablerle (3071)

Dim Shared As Integer mainIndex, auxIndex, cPoolIndex, tMapsIndex, defaultInk, rleIdx
Dim Shared As Integer spriteHeight
Dim Shared As Integer outputPatterns
Dim Shared As Integer lastWmeta, lasthMeta

Dim Shared As Integer platform
Dim Shared As Integer brickMultiplier
Dim Shared As Integer patternSize
Dim Shared As Integer patternWidthInPixels

Dim Shared As uInteger globalPalette (31), HWPalette (31)

' 255 is an out of bounds value meaning "undefined". 
Dim Shared As RGBType CPCHWColours (31) => { _
	(1, 1, 1), _
	(255,255,255), _
	(0, 2, 1), _
	(2, 2, 1), _
	(0, 0, 1), _
	(2, 0, 1), _
	(0, 1, 1), _
	(2, 1, 1), _
	(255,255,255), _
	(255,255,255), _
	(2, 2, 0), _
	(2, 2, 2), _
	(2, 0, 0), _
	(2, 0, 2), _
	(2, 1, 0), _
	(2, 1, 2), _
	(255,255,255), _
	(255,255,255), _
	(0, 2, 0), _
	(0, 2, 2), _
	(0, 0, 0), _
	(0, 0, 2), _
	(0, 1, 0), _
	(0, 1, 2), _
	(1, 0, 1), _
	(1, 2, 1), _
	(1, 2, 0), _
	(1, 2, 2), _
	(1, 0, 0), _
	(1, 0, 2), _
	(1, 1, 0), _
	(1, 1, 2) _
}

Dim Shared As Integer CPCFWColours (31) => {_
	13, _ 
	-1, _
	19, _
	25, _
	1, _
	7, _
	10, _
	16, _
	-1, _
	-1, _
	24, _
	26, _
	6, _
	8, _
	15, _
	17, _
	-1, _
	-1, _
	18, _
	20, _
	0, _
	2, _
	9, _
	11, _
	4, _
	22, _
	21, _
	23, _
	3, _
	5, _
	12, _
	14 _
}

Sub fiPuts (s As String)
	If Not silent Then Puts s
End Sub

Sub usage
	Puts "Usage:"
	Puts ""
	Puts "$ mkts_om.exe platform=[zx|cpc] [brickInput] [pal=palette.png] [prefix=prefix]"
	Puts "              in=file.png out=output.bin mode=mode [offset=x,y] [size=w,h]"
	Puts "              [metasize=w,h] [tmapoffs=offset] [max=n] [silent] [defaultink=i]"
	Puts ""
	Puts "Supported modes: pals, chars, mapped, sprites, bg, scripted, scr"
	Puts "In scripted mode, parameter out will be ignored."
	Puts "brickinput means input png has 2x1 ""pixels""."
End Sub

Sub mbWrite (v As uByte)
	mainBin (mainIndex) = v
	mainIndex = mainIndex + 1
End Sub

Sub abWrite (v As uByte)
	auxBin (auxIndex) = v
	auxIndex = auxIndex + 1
End Sub

Function cpcReduceToThird (v As Integer) As Integer
	Dim As Integer res
	If v < 85 Then 
		res = 0
	ElseIf v < 170 Then
		res = 1
	Else
		res = 2
	End If
	cpcReduceToThird = res
End Function

Function cpcPromoteTo8Bit (v As Integer) As Integer
	Select Case v
		Case 0: cpcPromoteTo8Bit = 0
		Case 1: cpcPromoteTo8Bit = 128
		Case 2: cpcPromoteTo8Bit = 255
	End Select
End Function

Function cpcHWColour (c As Integer) As Integer
	' Reads R, G, B and returns a CPC HW colour.

	Dim As Integer r, g, b, i, res

	r = RGBA_R (c): g = RGBA_G (c): b = RGBA_B (c)
	r = cpcReduceToThird (r)
	g = cpcReduceToThird (g)
	b = cpcReduceToThird (b)

	res = 0
	For i = 0 To 31
		If 	CPCHWColours (i).r = r And _
			CPCHWColours (i).g = g And _
			CPCHWColours (i).b = b Then
			res = i: Exit For
		End If
	Next i

	cpcHWColour = res
End Function

Function cpcNormalizeColour (Byval c As Integer) As Integer
	Dim As Integer res
	Dim As Integer r, g, b

	r = RGBA_R (c): g = RGBA_G (c): b = RGBA_B (c)

	r = cpcPromoteTo8Bit (cpcReduceToThird (r))
	g = cpcPromoteTo8Bit (cpcReduceToThird (g))
	b = cpcPromoteTo8Bit (cpcReduceToThird (b))

	cpcNormalizeColour = RGB (r, g, b)
End Function

Function palIndex (Byval c As Integer, pal () As Integer) As Integer
	Dim As Integer i, res
	res = &HFF
	For i = lBound (pal) To uBound (pal)
		If pal (i) = c Then res = i: Exit For
	Next i
	' If res = &HFF Then fiPuts "Colour " & c & " not found in pal!"
	palIndex = res
End Function

Function speccyColour (c As Integer) As Integer
	' Returns 0-7 (bright 0) or 9-15 (bright 1). Decode on target.
	Dim As Integer res, bright, r, g, b

	r = RGBA_R (c): g = RGBA_G (c): b = RGBA_B (c)

	bright = ((r > 224) Or (g > 224) Or (b > 224))
	If bright Then bright = 8

	res = bright
	If b >= 192 Then res = res + 1
	If r >= 192 Then res = res + 2
	If g >= 192 Then res = res + 4

	speccyColour = res
End Function

Function isBright (c As Integer) As Integer
	isBright = (c > 7)
End Function

Function getAttr (x0 As Integer, y0 As Integer, img As Any Ptr, ByRef c1 As Integer, ByRef c2 As Integer) As uByte
	Dim As uByte res
	Dim As Integer x, y

	c1 = speccyColour (Point (x0, y0, img))
	For y = 0 To 7
		For x = 0 To 7
			c2 = speccyColour (Point (x0 + x, y0 + y, img))
			If c2 <> c1 Then exit For
		Next x
		If c2 <> c1 Then Exit For
	Next y

	' If c2 = c1, complimentary paper
	If (c1 And 7) = (c2 And 7) Then 
		If defaultInk = -1 Then 
			If (c2 And 7) < 4 Then c1 = 7 Else c1 = 0
		Else
			c1 = defaultInk
		End If
	End If

	' Darker always paper
	If (c1 And 7) < (c2 And 7) Then Swap c1, c2

	res = (c1 And 7) Or ((c2 And 7) Shl 3)
	If isBright (c1) Or isBright (c2) Then res = res Or 64

	getAttr = res
End Function

Function getPaper (attr As uByte) As uByte
	getPaper = (attr Shr 3) And 7
End Function

Function getInk (attr As uByte) As uByte
	getInk = attr And 7
End Function

Function getBright (attr As uByte) As uByte
	getBright = (attr Shr 6) And 1
End Function

Function getBit (v As uByte, b As uByte) As uByte
	If (v And (1 Shl b)) <> 0 Then getBit = 1 Else getBit = 0
End Function

Function zxGetBitmapFrom (x0 As Integer, y0 As Integer, c2 As Integer, img As Any Ptr) As uByte
	Dim As uByte res
	Dim As Integer x, c

	res = 0
	For x = 0 To 7
		c = speccyColour (Point (x + x0, y0, img))
		If c <> c2 Then res = res Or (1 Shl (7 - x))
	Next x

	zxGetBitmapFrom = res
End Function

Function cpcGetBitmapFrom (Byval x0 As Integer, Byval y0 As Integer, img As Any Ptr) As uByte
	' Will convert two pixels.
	Dim As uByte c1, c2

	' Read two pixels
	c1 = palIndex (cpcNormalizeColour (Point (x0, y0, img)), globalPalette ())
	c2 = palIndex (cpcNormalizeColour (Point (x0 + brickMultiplier, y0, img)), globalPalette ())

	' PIXEL 0  PIXEL 1
	' 3 2 1 0  3 2 1 0
	' 1 5 3 7  0 4 2 6

	' Build garbled byte
	cpcGetBitmapFrom = _
		getBit (c2, 3) Or _
		(getBit (c1, 3) Shl 1) Or _
		(getBit (c2, 1) Shl 2) Or _
		(getBit (c1, 1) Shl 3) Or _
		(getBit (c2, 2) Shl 4) Or _
		(getBit (c1, 2) Shl 5) Or _
		(getBit (c2, 0) Shl 6) Or _
		(getBit (c1, 0) Shl 7)

		If debug Then
			Pset (x0,y0+200),c1*16
			Pset (x0+brickMultiplier,y0+200),c2*16
		End If
End Function

Sub extractPatternFrom (x0 As Integer, y0 As Integer, img As Any Ptr, pattern () As uByte, ByRef attr as uByte)
	Dim As Integer c1, c2
	Dim As Integer y

	Select Case platform
		Case PLATFORM_ZX:
			attr = getAttr (x0, y0, img, c1, c2)
			If debug Then fiPuts "c1 = " & c1 & ", c2 = " & c2
			For y = 0 To 7
				pattern (y) = zxGetBitmapFrom (x0, y + y0, c2, img)
			Next y

		Case PLATFORM_CPC:
			For y = 0 To 7
				pattern (y + y) = cpcGetBitmapFrom (x0, y + y0, img)
				pattern (y + y + 1) = cpcGetBitmapFrom (x0 + 2 * brickMultiplier, y + y0, img)
			Next y
	End Select		
End Sub

Function patternToString (pattern () As uByte) As String
	Dim As String res
	Dim As Integer i 

	res = ""
	For i = 0 To uBound (pattern)
		res = res & Hex (pattern (i), 2)
	Next i

	patternToString = res
End Function

Sub addPatternToPool (patternS As String)
	cPool (cPoolIndex) = patternS
	cPoolIndex = cPoolIndex + 1
	outputPatterns = outputPatterns + 1
End Sub

Sub copyArrayToMainBin (array () As uByte)
	Dim As Integer i
	For i = lBound (array) To uBound (array)
		mbWrite array (i)
	Next i
End Sub

Sub copyPartialArrayToMainBin (array () As uByte, size As Integer)
	Dim As Integer i
	For i = lBound (array) To lBound (array) + size - 1
		mbWrite array (i)
	Next i
End Sub

Function findPatternInPoolAndAdd (patternS As String, ByRef wasNew As Integer) As Integer
	Dim As Integer i

	wasNew = 0

	For i = 0 To cPoolIndex - 1
		If cPool (i) = patternS Then
			findPatternInPoolAndAdd = i
			Exit Function
		End If
	Next i

	wasNew = -1
	findPatternInPoolAndAdd = cPoolIndex
	addPatternToPool patternS
End Function

Sub extractGlobalPalette (img As Any Ptr) 
	Dim As Integer i
	Select Case platform
		Case PLATFORM_CPC:
			For i = 0 To 15
				globalPalette (i) = cpcNormalizeColour (Point (i, 0, img))
				HWPalette (i) = cpcHWColour (globalPalette (i))
			Next i
	End Select
End Sub

Sub doPals (img As Any Ptr, prefix As String, outputFn As String)
	Dim As Integer i, f, c

	extractGlobalPalette img

	Select Case platform
		Case PLATFORM_CPC:
			' Write HW colours
			f = FreeFile
			Open outputFn For Output As #f
			Print #f, "// CPC Palette with HW values"
			Print #f, "// Generated by mkts_om v0.2.20180904"
			Print #f, "// Copyleft 2017 by The Mojon Twins"
			Print #f, ""
			Print #f, "const unsigned char " & prefix & " [] = {"
			For i = 0 To 15
				c = globalPalette (i)
				Print #f, "	";
				Print #f, "0x" & Hex (cpcHWColour (c), 2);
				If i < 15 Then Print #f, ", "; Else Print #f, "  ";
				Print #f, "// " & Hex (c, 8)
			Next i
			Print #f, "};"
			Print #f, ""
			Close #f
	End Select
End Sub

Sub doChars (img As Any Ptr, xc0 As Integer, yc0 As Integer, w As Integer, h As Integer, max As Integer)
	Dim As Integer x, y, x0, y0, x1, y1, ct
	Dim As uByte attr 	' will be ignored in this Sub
	Dim As uByte pattern (15)

	x0 = xc0 * patternWidthInPixels
	y0 = yc0 * 8
	x1 = x0 + w * patternWidthInPixels - 1
	y1 = x0 + h * 8 - 1
	ct = 0

	For y = y0 To y1 Step 8
		For x = x0 To x1 Step patternWidthInPixels
			extractPatternFrom x, y, img, pattern (), attr
			addPatternToPool patternToString (pattern ())
			if debug Then puts patternToString (pattern ())
			copyPartialArrayToMainBin pattern (), patternSize
			ct = ct + 1
			If max <> -1 And ct = max Then Exit For
		Next x
		If max <> -1 And ct = max Then Exit For
	Next y

	Puts "mkts_om v0.2.20180904 ~ Chars mode, " & ct & " patterns extracted (" & (ct * patternSize) & " bytes)."
End Sub

Sub cpcDoSuperBuffer (img As Any Ptr)
	Dim As Integer x, y

	For y = 0 To 191
		For x = 0 To 63
			mbWrite cpcGetBitmapFrom (x * 2 * brickMultiplier, y, img)
		Next x
	Next y

	Puts "mkts_om v0.2.20180904 ~ Superbuffer mode (12288 bytes)."
End Sub

Sub cpcDoScr (img As Any Ptr)
	Dim As Integer x, y, yy, i
	Dim As String textpal

	For yy = 0 To 7
		For y = 0 To 192 Step 8
			For x = 0 To 79
				mbWrite cpcGetBitmapFrom (x * 2 * brickMultiplier, y + yy, img)
			Next x
		Next y
		' Pad 48 bytes per eighth
		For x = 0 To 47: mbWrite 0: Next x
	Next yy

	Puts "mkts_om v0.2.20180904 ~ Superbuffer mode (16384 bytes)."
	Puts "HW Palette for the ASM loader: "
	textpal = ""
	For i = 0 To 15
		textpal = textpal & HWPalette (i)
		If i < 15 Then textpal = textpal & ", "
	Next i
	Puts textpal 
	Puts "FW Palette for the BASIC loader: "
	textpal = ""
	For i = 0 To 15
		textpal = textpal & CPCFWColours (HWPalette (i)) 
		If i < 15 Then textpal = textpal & ", "
	Next i
	Puts textpal 
End Sub

Sub doScr (img As Any Ptr)
	Select Case platform
		Case PLATFORM_CPC
			cpcDoScr img
		Case PLATFORM_ZX
			Puts "not supprted yet"
	End Select
End Sub

Sub doTmaps (img As Any Ptr, xc0 As Integer, yc0 As Integer, w As Integer, h As Integer, wMeta As Integer, hMeta As Integer, max As Integer)
	Dim As Integer x, y, x0, y0, x1, y1, ct, ctTmaps, xa, ya, i
	Dim As Integer xx, yy
	Dim As uByte attr 	' will be ignored in this Sub
	Dim As uByte pattern (15)
	Dim As Integer cTMapIndex
	Dim As Integer wasNew, pIndex

	lastwMeta = wMeta
	lasthMeta = hMeta

	x0 = xc0 * patternWidthInPixels
	y0 = yc0 * 8
	x1 = x0 + w * wMeta * patternWidthInPixels - 1
	y1 = x0 + h * hMeta * 8 - 1
	ct = 0
	tMapsIndex = 0
	ctTmaps = 0
	
	For y = y0 To y1 Step 8 * wMeta
		For x = x0 To x1 Step patternWidthInPixels * hMeta
			
			' Read chars in current metatile
			cTMapIndex = 0
			ya = y
			For yy = 0 To hMeta - 1
				xa = x
				For xx = 0 To wMeta - 1
					extractPatternFrom xa, ya, img, pattern (), attr
					pIndex = findPatternInPoolAndAdd (patternToString (pattern ()), wasNew)
					If wasNew Then copyPartialArrayToMainBin pattern (), patternSize: ct = ct + 1

					' Add to current tmap
					If platform = PLATFORM_ZX Then
						tMaps (tMapsIndex, cTMapIndex) = attr: cTMapIndex = cTMapIndex + 1
					End If						
					tMaps (tMapsIndex, cTMapIndex) = pIndex: cTMapIndex = cTMapIndex + 1

					xa = xa + patternWidthInPixels
				Next xx

				ya = ya + 8
			Next yy

			tMapsIndex = tMapsIndex + 1: ctTmaps = ctTmaps + 1
		Next x
	Next y

	i = 1: If platform = PLATFORM_ZX Then i = 2
	Puts "mkts_om v0.2.20180904 ~ Tmaps mode, " & ct & " patterns extracted (" & (ct * patternSize) & " bytes) from " & ctTmaps & " metatiles (" & (ctTmaps * i * wMeta * hMeta) & " bytes)."
End Sub

Sub zxDoSprites (img As Any Ptr, xc0 As Integer, yc0 As Integer, w As Integer, h As Integer, wMeta As Integer, hMeta As Integer, max As Integer)
	Dim As Integer x, y, x0, y0, x1, y1, ct, ctTmaps, xa, ya, i
	Dim As Integer xx, yy
	Dim As uByte attr 	' will be ignored in this Sub
	Dim As Integer cTMapIndex
	Dim As Integer wasNew, pIndex
	Dim As Integer wMetaPixels, hMetaPixels

	' Still only masked sprites
	' Note that "zero" colour must be pitch black!

	x0 = xc0 * 8
	y0 = yc0 * 8
	wMetaPixels = wMeta * 8
	hMetaPixels = hMeta * 8
	x1 = x0 + w * wMetaPixels * 2 - 1
	y1 = x0 + h * hMetaPixels - 1
	ct = 0

	For y = y0 To y1 Step hMetaPixels
		For x = x0 To x1 Step wMetaPixels * 2

			' wMeta + 1 columns. The last will be empty
			xa = x
			For xx = 0 To wMeta
				If xx < wMeta Then
					' hMetaPixels sprite lines
					For yy = 0 To hMetaPixels - 1
						' Graphic 
						mbWrite zxGetBitmapFrom (xa, y + yy, 0, img)
						' Mask is 16 pixels appart
						mbWrite zxGetBitmapFrom (xa + wMetaPixels, y + yy, 0, img)
					Next yy
				Else
					' hMetaPixels empty lines for the last column
					For i = 0 To hMetaPixels - 1
						mbWrite 0: mbWrite 255
					Next i
				End If

				' 8 empty lines
				For i = 0 To 7
					mbWrite 0: mbWrite 255
				Next i

				xa = xa + 8
			Next xx

			ct = ct + 1
			If max > 0 And ct = max Then goto zxDoSpritesBreak
		Next x
	Next y
zxDoSpritesBreak:

	Puts "mkts_om v0.2.20180904 ~ Sprites mode, " & ct & " " & wMetaPixels & "x" & hMetaPixels & " sprite cells with masks extracted (" & (mainIndex) & " bytes)."
End Sub

Sub cpcDoSprites (img As Any Ptr, xc0 As Integer, yc0 As Integer, w As Integer, h As Integer, wMeta As Integer, hMeta As Integer, max As Integer)
	Dim As Integer x, y, x0, y0, x1, y1, ct, ctTmaps, xa, ya, i
	Dim As Integer xx, yy
	Dim As uByte attr 	' will be ignored in this Sub
	Dim As Integer cTMapIndex
	Dim As Integer wasNew, pIndex
	Dim As Integer wMetaPixels, hMetaPixels
	Dim As Integer curIndex

	curIndex = mainIndex

	x0 = xc0 * patternWidthInPixels
	y0 = yc0 * 8
	wMetaPixels = wMeta * patternWidthInPixels
	hMetaPixels = hMeta * 8
	x1 = x0 + w * wMetaPixels - 1
	y1 = x0 + h * hMetaPixels - 1
	ct = 0

	For y = y0 To y1 Step hMetaPixels
		For x = x0 To x1 Step wMetaPixels
			For yy = 0 To hMetaPixels - 1
				xa = x
				For xx = 0 To wMeta - 1
					mbWrite cpcGetBitmapFrom (xa, y + yy, img)
					mbWrite cpcGetBitmapFrom (xa + 2 * brickMultiplier, y + yy, img)
					xa = xa + patternWidthInPixels
				Next xx
			Next yy
			ct = ct + 1
			If max > 0 And ct = max Then goto cpcDoSpritesBreak
		Next x
	Next y
cpcDoSpritesBreak:

	Puts "mkts_om v0.2.20180904 ~ Sprites mode, " & ct & " " & wMetaPixels & "x" & hMetaPixels & " sprite cells extracted (" & (mainIndex - curIndex) & " bytes)."
End Sub

Sub doSprites (img As Any Ptr, xc0 As Integer, yc0 As Integer, w As Integer, h As Integer, wMeta As Integer, hMeta As Integer, max As Integer)
	Select Case platform 
		Case PLATFORM_ZX:
			zxDoSprites img, xc0, yc0, w, h, wMeta, hMeta, max
		Case PLATFORM_CPC
			cpcDoSprites img, xc0, yc0, w, h, wMeta, hMeta, max
	End Select
End Sub

Sub zxDoBg (img As Any Ptr, xc0 As Integer, yc0 As Integer, w As Integer, h As Integer)
	Dim As Integer x, y, x0, y0, x1, y1, i
	Dim As uByte pattern (7)
	Dim As uByte attr, pIndex
	Dim As Integer wasNew, ct
	Dim As Integer baseIndex, patternTableSize

	x0 = xc0 * 8
	y0 = yc0 * 8
	x1 = x0 + w * 8 - 1
	y1 = y0 + h * 8 - 1

	auxIndex = 0
	baseIndex = mainIndex: mainIndex = mainIndex + 2
	ct = 0

	For y = y0 To y1 Step 8
		For x = x0 To x1 Step 8
			extractPatternFrom x, y, img, pattern (), attr
			pIndex = findPatternInPoolAndAdd (patternToString (pattern ()), wasNew)
			If wasNew Then copyArrayToMainBin pattern (): ct = ct + 1
		
			abWrite attr
			abWrite pIndex
		Next x
	Next y

	' Write size to main Binary
	patternTableSize = mainIndex - baseIndex
	mainBin (baseIndex) = patternTableSize And &HFF
	mainBin (baseIndex + 1) = patternTableSize Shr 8

	' Copy nametable to main binary
	For i = 1 To auxIndex - 1 Step 2
		mbWrite auxBin (i)
	Next i
	For i = 0 To auxIndex - 2 Step 2
		mbWrite auxBin (i)
	Next i

	Puts "mkts_om v0.2.20180904 ~ BG mode, " & ct & " patterns (" & (8*ct) & " bytes). NT is " & auxIndex & " bytes."
End Sub

Sub cpcDoNametable (img As Any Ptr)
	Dim As Integer x, y, ct, wasNew
	Dim As uByte attr, pIndex
	Dim As uByte pattern (15)

	ct = 0
	For y = 0 To 191 Step 8
		For x = 0 To 32*patternWidthInPixels-1 Step patternWidthInPixels
			extractPatternFrom x, y, img, pattern (), attr
			pIndex = findPatternInPoolAndAdd (patternToString (pattern ()), wasNew)
			If wasNew Then copyPartialArrayToMainBin pattern (), patternSize: ct = ct + 1
			nametable ((y\8)*32+(x\patternWidthInPixels)) = pIndex
		Next x
	Next y

	Puts "mkts_om v0.2.20180904 ~ Nametable mode, " & ct & " patterns extracted (" & (ct * patternSize) & " bytes)."
End Sub

Sub zxDoNametable (img As Any Ptr)
	Dim As Integer x, y, ct, wasNew
	Dim As uByte attr, pIndex
	Dim As uByte pattern (15)

	ct = 0
	For y = 0 To 191 Step 8
		For x = 0 To 255 Step 8
			extractPatternFrom x, y, img, pattern (), attr
			pIndex = findPatternInPoolAndAdd (patternToString (pattern ()), wasNew)
			If wasNew Then copyPartialArrayToMainBin pattern (), patternSize: ct = ct + 1
			nametable ((y\8)*32+(x\8)) = pIndex
			nametable (768 + (y\8)*32+(x\8)) = attr
		Next x
	Next y	

	Puts "mkts_om v0.2.20180904 ~ Nametable mode, " & ct & " patterns extracted (" & (ct * patternSize) & " bytes)."
End Sub

Sub doNametable (img As Any Ptr) 
	Select Case platform
		Case PLATFORM_CPC
			cpcDoNametable img
		Case PLATFORM_ZX
			zxDoNametable img
	End Select
End Sub

Sub writeToNametablerle (b As uByte)
	nametablerle (rleIdx) = b
	rleIdx = rleIdx + 1
End Sub

Sub doNametableRLE (img As Any Ptr)
	Dim As Integer namIdx
	Dim As uByte rleByte
	Dim As uByte runByte
	Dim As Integer runMode
	Dim As uByte runLength

	Dim As Integer sizeT

	Select Case platform
		case PLATFORM_ZX: sizeT = 1536
		case PLATFORM_CPC: sizeT = 768
	End Select
	
	' call doNametable, then RLE nametable.
	doNametable img

	' RLE. Attempt to write a neslib RLE compatible stream
	' 0xff is the run marker in this simplification (can't be used!)
	rleIdx = 0
	writeToNametablerle &HFF

	namIdx = 0: rleByte = &HFF: runMode = 0
	While namIdx < sizeT
		If runMode Then
			' Read byte
			runByte = nametable (namIdx): namIdx = namIdx + 1
			If runByte = rleByte And runLength < 255 Then
				runLength = runLength + 1
			Else
				' Write runLength
				writeToNametablerle runLength
				runMode = 0
				' Output byte
				writeToNametablerle runByte
				rleByte = runByte
			End If
		Else
			' Read byte
			runByte = nametable (namIdx): namIdx = namIdx + 1
			If runByte <> rleByte Then
				' Output byte
				writeToNametablerle runByte
				rleByte = runByte
			Else
				' start run
				writeToNametablerle &HFF
				runLength = 1
				runMode = -1
			End If
		End If
	Wend

	If runMode Then writeToNametablerle runLength

	' Write end marker
	writeToNametablerle &HFF
	writeToNametablerle 0

	Puts "mkts_om v0.2.20180904 ~ Rle encoder, compressed to " & rleIdx & " bytes."
End Sub

Function writeBin (fOut As Integer, binArray () As uByte, binOffs As Integer, bytes As Integer) As Integer
	Dim As Integer i, upperBound
	upperBound = binOffs + bytes - 1
	If upperBound > uBound (binArray) Then upperBound = uBound (binArray)

	fiPuts "Writing " & bytes & " bytes to output."
	For i = binOffs To upperBound 
		Put #fOut, , binArray (i)
	Next i

	writeBin = upperBound - binOffs + 1
End Function

Sub writeFullBinary (fileName As String)
	Dim As Integer fOut
	Dim As Integer bytes

	fiPuts "Opening " & fileName & " for output."
	Kill fileName
	fOut = FreeFile
	Open fileName For Binary As #fOut
	bytes = writeBin (fOut, mainBin (), 0, mainIndex)
	Close #fOut
	fiPuts "+ " & bytes & " bytes written"
End Sub

Sub writeTsmaps (fileName As String)
	Dim As Integer fOut
	Dim As Integer bytes
	Dim As Integer i, j, mult
	Dim As uByte d

	If platform = PLATFORM_ZX Then mult = 2 Else mult = 1

	fiPuts "Opening " & fileName & " for output."
	Kill fileName
	fOut = FreeFile
	Open fileName For Binary As #fOut
	bytes = 0
	For i = 0 To tMapsIndex - 1
		For j = 0 To lastwMeta * lasthMeta * mult - 1
			bytes = bytes + 1
			d = tMaps (i, j): Put #fOut, , d
		Next j
	Next i				
	Close #fOut
	fiPuts "+ " & bytes & " bytes written"
End Sub

Sub writeNametable (fileName As String)
	Dim As Integer fOut
	Dim As Integer i
	Dim As Integer sizeT
	
	Select Case platform
		Case PLATFORM_ZX: sizeT = 1535
		Case PLATFORM_CPC: sizeT = 767
	End Select

	fiPuts "Opening " & fileName & " for output."
	Kill fileName
	fOut = FreeFile
	Open fileName For Binary As #fOut
	For i = 0 To sizeT
		Put #fOut, , nametable (i)
	Next i
	Close #fOut
	fiPuts "+ " & (sizeT + 1) & " bytes written"
End Sub

Sub writeNametableRle (fileName As String)
	Dim As Integer fOut
	Dim As Integer i
	
	fiPuts "Opening " & fileName & " for output."
	Kill fileName
	fOut = FreeFile
	Open fileName For Binary As #fOut
	For i = 0 To rleIdx - 1
		Put #fOut, , nametablerle (i)
	Next i
	Close #fOut
	fiPuts "+ " & rleIdx & " bytes written"
End Sub

Sub zxDoScripted (scriptFile As String)
	Dim As Integer fIn
	Dim As String lineIn
	Dim As String tokens (31)
	Dim As Integer xc0, yc0, w, h, wMeta, hMeta, max, imgOn, palOn
	Dim As Integer wIn, hIn
	Dim As Any Ptr img, palimg

	imgOn = 0: palOn = 0
	fiPuts "Executing script " & scriptFile

	fIn = FreeFile
	Open scriptFile For Input As #fIn
	While Not Eof (fIn)
		Do
			Line Input #fIn, lineIn
			lineIn = Trim (lineIn, Any Chr (9) + Chr (32))
		Loop While Not Eof (fIn) And lineIn = ""
		If debug Then Puts lineIn
		parseTokenizeString lineIn, tokens (), ",;" & Chr (9), "#"

		Select Case Lcase (tokens (0))
			Case "brickinput"
				If tokens (1) = "off" Then brickMultiplier = 1 Else brickMultiplier = 2
				fiPuts "Brick multiplier = " & brickMultiplier
				patternWidthInPixels = 4 * brickMultiplier

			Case "pal"
				fiPuts "Reading palette file " & tokens (1)
				If palOn Then ImageDestroy palimg
				palimg = png_load (tokens (1))
				palOn = -1
		
				If ImageInfo (palimg, wIn, hIn, , , , ) Then
					fiPuts "There was an error reading pal file. Shitty png?": End
				End If
				
				extractGlobalPalette palimg

			Case "defaultink"
				defaultInk = Val (tokens (1))

			Case "open"
				fiPuts "Reading input file " & tokens (1)
				If imgOn Then ImageDestroy img
				img = png_load (tokens (1))
				imgOn = -1

				If ImageInfo (img, wIn, hIn, , , , ) Then
					fiPuts "There was an error reading input file. Shitty png?": End
				End If

				If debug Then Put (0, 0), img, Pset: puts "KEY!":sleep
			Case "output"
				If tokens (1) = "patterns" Then writeFullBinary tokens (2)
				If tokens (1) = "tmaps" Then writeTsmaps tokens (2)
				If tokens (1) = "nametable" Then writeNametable tokens (2)
				If tokens (1) = "nametablerle" then writeNametableRle tokens (2)

			Case "reset"
				If tokens (1) = "patterns" Then mainIndex = 0
				If tokens (1) = "tmaps" Then tMapsIndex = 0

			Case "fillto"
				While mainIndex < Val (tokens (1))
					mbWrite 0
				Wend

			Case "spriteset"
				xc0 = Val (tokens (1))
				yc0 = Val (tokens (2))
				w = Val (tokens (3))
				h = Val (tokens (4))
				wMeta = Val (tokens (5))
				hMeta = Val (tokens (6))
				max = Val (tokens (7)): If max = 0 Then max = -1

				doSprites img, xc0, yc0, w, h, wMeta, hMeta, max

			Case "metatileset"
				xc0 = Val (tokens (1))
				yc0 = Val (tokens (2))
				w = Val (tokens (3))
				h = Val (tokens (4))
				wMeta = Val (tokens (5))
				hMeta = Val (tokens (6))
				max = Val (tokens (7)): If max = 0 Then max = -1
				
				doTmaps img, xc0, yc0, w, h, wMeta, hMeta, max

			Case "charset"
				xc0 = Val (tokens (1))
				yc0 = Val (tokens (2))
				w = Val (tokens (3))
				h = Val (tokens (4))
				max = Val (tokens (7)): If max = 0 Then max = -1
				
				doChars img, xc0, yc0, w, h, max

			Case "nametable"
				doNametable img

			Case "nametablerle"
				doNameTableRLE img

			Case "superbuffer"
				cpcDoSuperBuffer img

			Case "patternoffset"
				cPoolIndex = Val (tokens (1))

			Case "stats"
				Puts "stats: " & (mainIndex\patternSize) & " patterns in pool (" & mainIndex & " bytes)"

		End Select
	Wend

	If imgOn Then ImageDestroy img
	If palOn Then ImageDestroy palimg
				
End Sub

Dim As String mandatory (2) => { "in", "out", "mode" }
Dim As String mandatoryScripted (1) => { "in", "mode" }
Dim As Integer xc0, yc0, w, h, wMeta, hMeta, max, i, j
Dim As Integer coords (9)
Dim As Integer wIn, hIn
Dim As String outputBaseFn, fileName, prefix
Dim As Integer fOut, bytes
Dim As Any Ptr img
Dim As uByte d

sclpParseAttrs
cPoolIndex = 0

' Extra params.

flipped = (sclpGetValue ("genflipped") <> "")
silent = (sclpGetValue ("silent") <> "")
debug = (sclpGetValue ("debug") <> "")

For i = 0 To uBound (globalPalette)
	globalPalette (i) = RGB (&HFE, &HFE, &HFE)
Next i

' Hello
fiPuts "mkts_om v0.2.20180904"

' Mandatory params
If sclpGetValue ("mode") = "scripted" Then
	If Not sclpCheck (mandatoryScripted ()) Then usage: End
Else
	If Not sclpCheck (mandatory ()) Then usage: End
End If

' Bricks
brickMultiplier = 1
If sclpGetValue ("brickInput") <> "" Then brickMultiplier = 2

' We need to read the input image at this point
If debug Then
	screenres 640,480, 32
Else
	screenres 640, 480, 32, , -1
End If

' Select platform
If sclpGetValue ("platform") = "cpc" Then
	platform = PLATFORM_CPC
	patternSize = 16
	patternWidthInPixels = 4 * brickMultiplier

	If sclpGetValue ("mode") <> "scripted" And sclpGetValue ("mode") <> "pal" And sclpGetValue ("mode") <> "pals" Then
		If sclpGetValue ("pal") = "" Then
			Puts ("** pal file missing **")
			usage
			End
		End If
		
		fiPuts "Reading palette file " & sclpGetValue ("pal")
		img = png_load (sclpGetValue ("pal"))
		
		If ImageInfo (img, wIn, hIn, , , , ) Then
			fiPuts "There was an error reading input file. Shitty png?": End
		End If
		
		extractGlobalPalette img
	End If
Else
	platform = PLATFORM_ZX
	patternSize = 8
	patternWidthInPixels = 8
End If

If sclpGetValue ("mode") <> "scripted" Then
	fiPuts "Reading input file " & sclpGetValue ("in")
	img = png_load (sclpGetValue ("in"))

	If ImageInfo (img, wIn, hIn, , , , ) Then
		fiPuts "There was an error reading input file. Shitty png?": End
	End If

	If debug Then 
		Put (0,0),img,PSET
	End If
End If

' offset
If sclpGetValue ("offset") <> "" Then
	parseCoordinatesString sclpGetValue ("offset"), coords ()
	xc0 = coords (0): yc0 = coords (1)
Else
	xc0 = 0: yc0 = 0
End If
fiPuts "Offset (" & xc0 & ", " & yc0 & ")"

' metasize
If sclpGetValue ("mode") = "chars" Then
	wMeta = 1: hMeta = 1
Else
	If sclpGetValue ("metasize") <> "" Then
		parseCoordinatesString sclpGetValue ("metasize"), coords ()
		wMeta = coords (0): hMeta = coords (1)
	Else
		wMeta = 2: hMeta = 2
	End If
	fiPuts "Metasize (" & wMeta & ", " & hMeta & ")"
End If

' size
If sclpGetValue ("size") <> "" Then
	parseCoordinatesString sclpGetValue ("size"), coords ()
	w = coords (0): h = coords (1)
Else
	w = wIn \ (wMeta * 8): h = hIn \ (hMeta * 8)
End If
fiPuts "Process size (" & w & ", " & h & ")"

' tmapoffs
cPoolIndex = Val (sclpGetValue ("tmapoffs"))
fiPuts "tmapoffs " & cPoolIndex

' max
max = Val (sclpGetValue ("max")): If max < 1 Then max = -1	' Which means no limit
fiPuts "max " & max

' default Ink
If sclpGetValue ("defaultink") <> "" Then 
	defaultInk = Val (sclpGetValue ("defaultink"))
Else
	defaultInk = -1
End If

' Fix output
outputBaseFn = sclpGetValue ("out")
If Instr (outputBaseFn, ".bin") Then outputBaseFn = Left (outputBaseFn, Instr (outputBaseFn, ".bin") - 1)
If Instr (outputBaseFn, ".h") Then outputBaseFn = Left (outputBaseFn, Instr (outputBaseFn, ".h") - 1)

' Prefix
prefix = sclpGetValue ("prefix")
fiPuts "with prefix " & prefix

' And do.

Select Case sclpGetValue ("mode")
	Case "pal", "pals"
		If Trim (prefix) = "" Then prefix = "my_inks"
		doPals img, prefix, outputBaseFn & ".h"
		ImageDestroy img

	Case "chars"
		doChars img, xc0, yc0, w, h, max
		writeFullBinary outputBaseFn & ".bin"
		ImageDestroy img

	Case "mapped"
		doTmaps img, xc0, yc0, w, h, wMeta, hMeta, max
		writeFullBinary outputBaseFn & ".patterns.bin"
		writeTsmaps outputBaseFn & ".tilemaps.bin" 
		ImageDestroy img

	Case "sprites"
		doSprites img, xc0, yc0, w, h, wMeta, hMeta, max
		writeFullBinary outputBaseFn & ".bin"
		ImageDestroy img

	Case "bg"
		zxDoBg img, xc0, yc0, w, h
		writeFullBinary outputBaseFn & ".bin"
		ImageDestroy img

	Case "nametable"
		doNameTable img
		writeFullBinary outputBaseFn & ".bin"
		writeNametable outputBaseFn & ".nam"

	Case "nametablerle"
		doNameTableRLE img
		writeFullBinary outputBaseFn & ".bin"
		writeNametableRle outputBaseFn & ".rle"

	Case "superbuffer"
		cpcDoSuperBuffer img
		writeFullBinary outputBaseFn & ".bin"

	Case "scr"
		doScr img
		writeFullBinary outputBaseFn & ".bin"

	Case "scripted"
		zxDoScripted sclpGetValue ("in")

End Select
				
fiPuts "DONE"

If debug Then Puts "Any key to exit": Sleep: End

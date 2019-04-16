; [00] Fanfare 2 (todo)
SFX00:		DB	$1F,$0B
				DB	$AF,$0F
				DB	$8A,$0F
				DB	$71,$0F
				DB	$64,$0F
				DB	$3E,$0C
				DB	$25,$0C
				DB	$25,$0C
				DB	$25,$0C
				DB	$25,$0A
				DB	$4B,$0A
				DB	$4B,$0A
				DB	$4B,$0A
				DB	$3E,$08
				DB	$3E,$08
				DB	$3E,$08
				DB	$71,$08
				DB	$3E,$07
				DB	$25,$05
				DB	$25,$02
				DB	$FF

; [1] Sartar JUMP (Alternate) (cambiar!)
SFX01:		DB	$E8,$1B
				DB	$B4,$0F
				DB	$A0,$0E
				DB	$90,$0D
				DB	$87,$0D
				DB	$78,$0C	
				DB	$6C,$0B	
				DB	$60,$0A	
				DB	$5A,$09
				DB	$FF					
				
; [2] Select
SFX02:		DB 	$51,$1A
				DB	$5A,$0F
				DB	$3C,$0F
				DB	$1E,$0E
				DB	$2D,$E
				DB	$5A,$0B
				DB	$3C,$0B
				DB	$1E,$0A
				DB	$2D,$0A
				DB	$B4,$01
				DB	$FF
				
; [3] Objeto
SFX03:		DB	$1F,$0B
				DB	$1A,$0C
				DB	$1F,$0D
				DB	$16,$0E
				DB	$1F,$0E
				DB	$0D,$0D
				DB	$1F,$0C
				DB	$0D,$0B
				DB	$00,$00
				DB	$00,$00
				DB	$1F,$08
				DB	$1A,$09
				DB	$1F,$0A
				DB	$16,$0B
				DB	$1F,$0B
				DB	$0D,$0A
				DB	$1F,$09
				DB	$0D,$07
				DB	$00,$00
				DB	$00,$00
				DB	$1F,$06
				DB	$1A,$07
				DB	$1F,$08
				DB	$16,$08
				DB	$1F,$07
				DB	$0D,$06
				DB	$1F,$05
				DB	$FF
				
; [4] Puncho
SFX04:			DB 	$E8,$1B
				DB	$15F,$0F
				DB	$2A6,$0F
				DB	$00,$00
				DB 	$80,$0F
				DB	$FF	
	
; [5] Hit
SFX05:			DB 	$51,$1A
				DB 	$E8,$1B
				DB 	$80,$2B
				DB 	$FF   

; [6] Fire		
SFX06:			DB 	$25,$1C
				DB 	$30,$2E
				DB	$00,$00
				DB	$A8,$0A
				DB	$C5,$1A
				DB	$FF
				
; [7] No ammo fire		
SFX07:			DB 	$25,$1C
				DB	$FF

; [8] OK / Start
SFX08:			DB 	$25,$1C
				DB 	$3A,$0F
				DB	$2D,$0F
				DB	$E2,$0F
				DB	$BC,$0F
				DB	$96,$0D
				DB	$4B,$0D
				DB	$32,$0D
				DB 	$3A,$0D
				DB	$2D,$0D
				DB	$E2,$0D
				DB	$BC,$0D
				DB	$96,$0D
				DB	$4B,$0D
				DB	$32,$0D
				DB 	$3A,$0D
				DB	$2D,$0C
				DB	$E2,$0C
				DB	$BC,$0C
				DB	$96,$0B
				DB	$4B,$0B
				DB	$32,$0B
				DB 	$3A,$0B
				DB	$2D,$0B
				DB	$E2,$0B
				DB	$BC,$0B
				DB	$96,$0B
				DB	$4B,$0A
				DB	$32,$0A
				DB 	$3A,$0A
				DB	$2D,$09
				DB	$E2,$09
				DB	$BC,$08
				DB	$96,$08
				DB	$4B,$08
				DB	$32,$07
				DB 	$3A,$07
				DB	$2D,$06
				DB	$E2,$06
				DB	$BC,$06
				DB	$96,$05
				DB	$4B,$05
				DB	$32,$05
				DB 	$3A,$04
				DB	$2D,$04
				DB	$E2,$03
				DB	$BC,$03
				DB	$96,$03
				DB	$4B,$03
				DB	$32,$02
				DB 	$3A,$01
				DB	$2D,$01
				DB	$E2,$01
				DB	$BC,$01
				DB	$FF
				
; [9] Text
SFX09:		DB 	$E8,$1B
				DB	$15F,$0F
				DB	$2A6,$0F
				DB	$00,$00
				DB 	$80,$0F
				DB	$FF	
				
; [10] Cosas
SFX10:		DB	$1F,$0B
				DB	$5A,$0F
				DB	$3C,$0F
				DB	$1E,$0A
				DB	$2D,$0A
				DB	$5A,$05
				DB	$3C,$05
				DB	$1E,$04
				DB	$2D,$02
				DB	$B4,$01
				DB	$FF
	
; [11] Disparo
SFX11:		DB	$1F,$0B
				DB	$AF,$0F
				DB	$8A,$0F
				DB	$71,$0F
				DB	$64,$0F
				DB	$3E,$0C
				DB	$25,$0C
				DB	$25,$0C
				DB	$25,$0C
				DB	$25,$0A
				DB	$4B,$0A
				DB	$4B,$0A
				DB	$4B,$0A
				DB	$3E,$08
				DB	$3E,$08
				DB	$3E,$08
				DB	$71,$08
				DB	$3E,$07
				DB	$25,$05
				DB	$25,$02
				DB	$FF
				
; [12] Salto largo
SFX12:		DB	$C3,$0E
				DB	$CC,$0D
				DB	$D5,$0A
				DB	$DE,$06
				DB	$35,$03
				DB	$50,$0B
				DB	$47,$0C
				DB	$3E,$08
				DB	$FF
	
; [13] Kill Enemy
SFX13:		DB 	$E8,$1B
				DB	$15F,$0F
				DB	$0E2,$0F
				DB	$356,$0F
				DB	$1F6,$0F
				DB	$114,$0E
				DB	$064,$0E	
				DB	$362,$0D
				DB	$1D0,$0D
				DB	$2F1,$0C
				DB	$FF	
				
; [14] Fanfare (to do!)
SFX14:		DB	$1A,$0E
				DB	$B4,$0E
				DB	$B4,$0E
				DB	$B4,$0E
				DB	$B4,$0E
				DB	$B4,$0E
				DB	$B4,$0E
				DB	$B4,$0E
				DB	$B4,$0E	
				DB	$A0,$0E
				DB	$A0,$0E
				DB	$A0,$0E
				DB	$A0,$0E
				DB	$A0,$0E
				DB	$A0,$0E
				DB	$A0,$0E
				DB	$87,$0E
				DB	$87,$0E
				DB	$87,$0E
				DB	$87,$0E
				DB	$87,$0E
				DB	$87,$0E
				DB	$87,$0E
				DB	$87,$0E
				DB	$87,$0E		
				DB	$78,$0E
				DB	$78,$0E
				DB	$78,$0D
				DB	$78,$0D
				DB	$78,$0D
				DB	$78,$0D
				DB	$78,$0D
				DB	$78,$0D
				DB	$78,$0C
				DB	$78,$09
				DB	$78,$06
				DB	$78,$05	
				DB	$FF
				
				
; [15] Gota (todo)
SFX15:		DB	$C3,$0E
				DB	$15F,$0F
				DB	$2A6,$0F
				DB 	$E8,$1B
				DB 	$80,$2B
				DB	$FF

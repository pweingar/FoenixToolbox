(define memories
  '(
	(memory flash
		(address (#xfc0000 . #xffdfff))
		(type ROM)
		(fill #xff)
	    (section
			data_init_table
			data
			ifar))

	(memory flash-high
		(address (#xffe000 . #xfffeff))
		(type ROM)
		(fill #xff)
		(section jumptable))

	(memory flash-shadow
		(address (#xffff00 . #xffffff))
		(type ROM)
		(fill 0)
	    (section
			(LoCodeStorage #xffff00)
		    (VectorStorage #xffffe0)))

    (memory LoCode
		(address (#x00ff00 . #x00ffdf))
		(type ROM)
		(scatter-to LoCodeStorage)
        (section code cdata))

    (memory Vector
		(address (#x00ffe0 . #x00ffff))
		(type ROM)
		(scatter-to VectorStorage)
    	(section (reset #xfffc)))

	(memory hiram
		(address (#x0f0000 . #x0fffff))
		(type RAM)
		(section heap))

	(memory loram
		(address (#x00d000 . #x00dfff))
		(type RAM)
		(placement-group nobits (section zdata znear))
		(placement-group bits (section data near)))

	(memory stackram
		(address (#x00edeb . #xfdeb))
		(type RAM)
		(section stack))

    (memory DirectPage
		(address (#x00fe00 . #x00feff))
        (section (registers ztiny)))

    (block stack (size #x1000))
    (block heap (size #x1000))
    (base-address _DirectPageStart DirectPage 0)
    (base-address _NearBaseAddress hiram   0)
))

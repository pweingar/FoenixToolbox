(define memories
  '(
	(memory flash
		(address (#xf80000 . #xfdffff))
		(type ROM)
		(fill #xff)
	    (section
			(jumptable #xf80000)
			data_init_table
			data
			ifar))

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
		(address (#x070000 . #x07ffff))
		(type RAM)
		(section heap))

	(memory loram
		(address (#x00d000 . #x00dfff))
		(type RAM)
		(placement-group nobits (section zdata znear))
		(placement-group bits (section data near)))

	(memory stackram
		(address (#x00c000 . #x00cfff))
		(type RAM)
		(section stack))

    (memory DirectPage
		(address (#x00bf00 . #x00bfff))
        (section (registers ztiny)))

    (block stack (size #x1000))
    (block heap (size #x1000))
    (base-address _DirectPageStart DirectPage 0)
    (base-address _NearBaseAddress hiram   0)
))

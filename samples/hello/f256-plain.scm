(define memories
  '(
	(memory LoMem
  		(address (#x0000 . #xbfff))
		(type ANY))
    (memory Vector (address (#xffe4 . #xffff)))
    (memory Banks
		(address (#x10000 . #x7ffff))
		(type ANY)
		(section (header #x10000)))
    (memory DirectPage
		(address (#x00c000 . #x00c0ff))
        (section (registers ztiny)))

    (block stack (size #x1000))
    (block heap (size #x1000))
    (base-address _DirectPageStart DirectPage 0)
    (base-address _NearBaseAddress LoMem 0)
  ))

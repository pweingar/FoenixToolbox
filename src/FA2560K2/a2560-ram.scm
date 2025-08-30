(define memories
  '((memory flash (address (#x1e0000 . #x1fffff)) (type ROM))
    (memory dataRAM (address (#x1c0000 . #x1dffff)) (type RAM))
    (memory Vector (address (#x0000 . #x03ff))
		(section (reset #x0000)))))

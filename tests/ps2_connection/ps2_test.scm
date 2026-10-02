(define memories
'((memory flash (address (#x100000 . #x1fffff)) (type ROM))
(memory RAM (address (#x200000 . #x23ffff)) (type RAM))
(memory Vector (address (#x0000 . #x03ff))
(section (reset #x0000)))
))

func `+`*(p: ptr, offset: int): type(p) {.inline.} =
    {.emit: "`result` = `p` + `offset`;".}

func `-`*(p: ptr, offset: int): type(p) {.inline.} =
    {.emit: "`result` = `p` - `offset`;".}

func `-`*(p1, p2: ptr): int {.inline.} =
    {.emit: "`result` = `p1` - `p2`;".}

template `+=`*[T](p: var ptr T, offset: int) =
    p = p + offset

template `-=`*[T](p: var ptr T, offset: int) =
    p = p - offset

template `[]`*[T](p: ptr T, offset: int): T =
    (p + offset)[]

template `[]=`*[T](p: ptr T, offset: int, val: T) = 
    (p + offset)[] = val
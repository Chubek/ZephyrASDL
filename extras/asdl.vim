" Vim syntax file
" Language: ASDL
" Author: Chubak Bidpaa

if exists("b:current_syntax")
    finish
endif

" The keywords for asdl(1) -- the superset
syn keyword asdlKeyword int uint int8 uint8 int16 uint16 int32 uint32 int64 uint64
syn keyword asdlKeyword char uchar
syn keyword asdlKeyword size usize
syn keyword asdlKeyword float32 float64 float80
syn keyword asdlKeyword boolean string identifier bytearray
syn keyword asdlKeyword attributes


" The operators (basically to look 'good'!)
syn match asdlOperator "\v[|;?*=:,()]"
syn match asdlOperator "::="

" Comments
syn match asdlComment "\v#.*$"

" Constructor identifiers
syn match asdlConIdent "\v[A-Z][a-zA-Z0-9_-]*"

" Type identifiers
syn match asdlTypeIdent "\v[a-z][a-z0-9_-]*"

" LHS identifiers
syn match asdlLhsIdent "\v^[a-z][a-z0-9_-]*"

" Linking
hi link asdlKeyword Keyword
hi link asdlComment Comment
hi link asdlOperator Operator
hi link asdlConIdent Identifier
hi link asdlTypeIdent Type
hi link asdlLhsIdent Underlined

let b:current_syntax = "asdl"



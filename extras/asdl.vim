" Vim syntax file
" Language: ASDL
" Author: Chubak Bidpaa

if exists("b:current_syntax")
    finish
endif

syn include @asdlCcode syntax/c.vim

" Embeds
syn region asdlPrelude start="%{" end="%}" contains=@asdlCcode
syn region asdlAppendage start="%%" end="\%$" contains=@asdlCcode
syn region asdlCAppend start="$" end="$" contains=asdlPrelude,asdlAppendage
syn region asdlMainCode start="^\_.*%%" end="%%" contains=asdlOperator,asdlKeyword,asdlComment,asdlConIdent,asdlTypeIdent,asdlLhsIdent



" The keywords for asdl(1) -- the superset
syn keyword asdlKeyword int uint int8 uint8 int16 uint16 int32 uint32 int64 uint64
syn keyword asdlKeyword char uchar
syn keyword asdlKeyword size usize
syn keyword asdlKeyword float32 float64 float80
syn keyword asdlKeyword boolean string identifier


" The operators (basically to look 'good'!)
syn match asdlOperator "\v[|;?*=:,()]" contained
syn match asdlOperator "::=" contained

" Comments
syn match asdlComment "\v#.*$" contained

" Constructor identifiers
syn match asdlConIdent "\v[A-Z][a-zA-Z0-9_-]*" contained

" Type identifiers
syn match asdlTypeIdent "\v[a-z][a-z0-9_-]*" contained

" LHS identifiers
syn match asdlLhsIdent "\v^[a-z][a-z0-9_-]*" contained

" Linking
hi link asdlKeyword Keyword
hi link asdlComment Comment
hi link asdlOperator Operator
hi link asdlConIdent Identifier
hi link asdlTypeIdent Type
hi link asdlLhsIdent Underlined

let b:current_syntax = "asdl"

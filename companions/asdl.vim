" Vim syntax file
" Language: ASDL
" Author: [Your Name]

if exists("b:current_syntax")
    finish
endif

syn include @asdlCcode syntax/c.vim

" Embeds
syn region asdlPrelude start="%{" end="%}" contains=@asdlCcode contained
syn region asdlAppend start="^%%" end="\%$" contains=@asdlCcode

" The keywords for asdl(1) -- the superset
syn keyword asdlKeyword int uint 
syn keyword asdlKeyword size usize 
syn keyword asdlKeyword boolean string identifier


" The operators (basically to look 'good'!)
syn match asdlOperator "\v[|;?*=:]"

" Comments
syn match asdlComment "\v#.*$"

" Constructor identifiers
syn match asdlConIdent "\v[A-Z][a-zA-Z0-9_]*"

" Type identifiers
syn match asdlTypeIdent "\v[a-z][a-z0-9_]*"

" Linking
hi link asdlKeyword Keyword
hi link asdlComment Comment
hi link asdlOperator Operator
hi link asdlConIdent Identifier
hi link asdlTypeIdent Type

let b:curent_syntax = "asdl"

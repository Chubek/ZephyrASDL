" Vim syntax file
" Language: ASDL
" Author: Chubak Bidpaa

if exists("b:current_syntax")
    finish
endif

syntax region asdlPrelude start="%%\n" end="\n%%\n" contains=@ALL

syntax match asdlOperator "="
syntax match asdlOperator ":"
syntax match asdlOperator "|"

syntax match conId /[A-Z][a-zA-Z0-9_]*/
syntax match typeId /[a-z][a-z0-9_]*/

highlight link asdlPrelude SpecialComment
highlight link asdlOperator Operator
highlight link conId Identifier
highlight link typeId Type

let b:current_syntax = "asdl"


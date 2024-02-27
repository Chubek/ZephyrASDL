" Vim syntax file
" Language: ASDL
" Author: Chubak Bidpaa

if exists("b:current_syntax")
    finish
endif

syntax include @asdlCCode syntax/c.vim

syntax region asdlPrelude start="%{\n" end="\n%}\n" contains=@asdlCCode
syntax region asdlAppend start="\n%%\n" end="\%$" contains=@asdlCCode
syntax region asdlComment start="\s*#\s*" end="\n"

syntax match asdlOperator /\v[=|;,:]/

syntax match asdlConId /[A-Z][a-zA-Z0-9_]*/
syntax match asdlTypeId /[a-z][a-z0-9_]*/

syntax keyword asdlKeywords bool int uint size usize string identifier

highlight link asdlOperator Operator
highlight link asdlConId Identifier
highlight link asdlTypeId Type
highlight link asdlKeywords Keyword
highlight link asdlComment Comment

let b:current_syntax = "asdl"


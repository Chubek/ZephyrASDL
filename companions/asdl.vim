" Vim syntax file
" Language: ASDL
" Author: Chubak Bidpaa

if exists("b:current_syntax")
    finish
endif

syntax include @asdlCCode syntax/c.vim

syntax region asdlPrelude start="%{" end="%}" contains=@asdlCCode
syntax region asdlAppend start="\n%%\n" end="\%$" contains=CONTAINED,@asdlCCode
syntax region asdlComment start="#\s+" end="\n"

syntax match asdlOperator "="
syntax match asdlOperator ":"
syntax match asdlOperator "|"
syntax match asdlOperator ";"
syntax match asdlOperator ","

syntax match asdlConId /\v[A-Z][a-zA-Z0-9_]*/
syntax match asdlTypeId /\v[a-z][a-z0-9_]*/

syntax match asdlPcntPcnt "\n%%\n" contained

syntax keyword asdlKeywords bool int uint size usize string identifier

highlight link asdlOperator Operator
highlight link asdlConId Identifier
highlight link asdlTypeId Type
highlight link asdlKeywords Keyword
highlight link asdlPcntPcnt Todo
highlight link asdlComment Comment

let b:current_syntax = "asdl"


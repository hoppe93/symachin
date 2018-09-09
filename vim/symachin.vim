" Vim syntax file
" Language: symachin
" Maintainer: Mathias Hoppe
" Latest Revision: 19 August 2018

if exists("b:current_synax")
    finish
endif

let b:current_syntax = "symachin"

" Keywords
syn keyword symachinKeyword apply assert by define end group in include other print printn replace to
" Operators
syn match symachinOperator '->\|+\|-\|*\|=\|:\|;'

" Labels & references
syn match symachinLabel '\[\w\+\]'
syn match symachinReference '\$[A-Za-z0-9_.]\+'

" Comments
syn match symachinComment '#.*$'
syn region symachinMLComment start="/\*" end="\*/"

" Strings
syn region symachinString start='"' end='"'

" Null
syn keyword symachinNull 0

hi def link symachinKeyword     Statement
hi def link symachinOperator    Statement
hi def link symachinLabel       Identifier
hi def link symachinReference   Type
hi def link symachinComment     Comment
hi def link symachinMLComment   Comment
hi def link symachinString      Constant
hi def link symachinNull        Constant

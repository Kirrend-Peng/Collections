# include <stdio.h>
# include <stdlib.h>
# include <ctype.h>
# include <iostream>
# include <string.h>
# include <stdlib.h>
# include <vector>
using namespace std;

typedef char SStr[127];
struct STree{
  SStr type;
  SStr token;
  bool hasquote;
  SStr address;  // name()
  STree * left ;
  STree * right ;
};

struct NeedRP{
  int wait;
  NeedRP * next;
};

struct InstructionList{
  SStr name;
  SStr type;
  int argumentNum;
  
};
typedef NeedRP * NeedRp;
typedef STree * BTree;

struct DefinitionNode{
  SStr name;
  BTree head;
  bool isfunction; 
  DefinitionNode * next;
};

bool gturnon = false;
typedef DefinitionNode * DefinitionList;

struct Areavariable{
  DefinitionList list;
  DefinitionList linker;
  Areavariable * next;
};

struct Templambda{
  SStr name;
  BTree parameter;
  BTree statement;
  Templambda * next;
};

typedef Areavariable * AreavariableList;
typedef Templambda * TemplambdaList;
NeedRp gNeedRP = NULL;
SStr glasttype = "";
int gline = 1; // 行)
int gcolumn = 0; // 列) 
bool gevaluerror = false;  // proj2 )
bool gArriveEOF = false;
bool gerror = false;
bool gdontprint = false;
bool gnotlocalarea = true;
bool gnoreturnvalue = false;
AreavariableList gdeList = NULL;
bool gexit = false;
int gtestnum = 0;
int gcondnum = 0;
int gfunctionnum = 0;
TemplambdaList gtemplambdaList = NULL;
void PutbackToken( SStr token ) {
  if ( strcmp( token, "" ) != 0 && token[0] == '\"' ) {
    for (  int i = strlen( token )-1 ; i >= 0 ; --i ) {
      if ( token[i] == '\n' ) {
        cin.putback( 'n' );
        cin.putback( '\\' );
        --gcolumn;
      } // if
      else if ( token[i] == '\t' ) {
        cin.putback( 't' );
        cin.putback( '\\' );
        --gcolumn;
      } // else if
      else if ( token[i] == '\\' ) {
        cin.putback( '\\' );
        cin.putback( '\\' );
        --gcolumn;
      } // else if
      else if ( i != strlen( token )-1 && i != 0 && token[i] == '\"' ) {
        cin.putback( '\"' );
        cin.putback( '\\' );
        --gcolumn;
      } // else if          
      else
        cin.putback( token[i] );
      --gcolumn;
    } // for
  } // if
  else {
    for ( int i = strlen( token )-1 ; i >= 0  ; --i ) {
      cin.putback( token[i] ) ;
      --gcolumn; 
    } // for
  } // else
} // PutbackToken()

void Movetofront( SStr token, int loc ) {     // 將字串往前挪一格 )
  int replace;
  for ( replace = loc++ ; loc < strlen( token ) ; ++loc, ++replace ) {
    token[replace] = token[loc];
  } // for
  token[replace] = '\0'; 
} // Movetofront()

void GetNextToken( SStr token1 ) {
  char ch;
  bool gItoken = false; // getting token
  bool findDQ = false;
  SStr token;
  strcpy( token, "" );
  int loc = -1;
  char peekch;
  while ( cin.peek() != EOF ) {
    peekch = cin.peek();
    if ( peekch == '\"' ) {               // 處理字串)
      if ( gItoken ) {                   // " 前的token尚未處理時進入)
        gItoken = false;
        token[++loc] = '\0';
        strcpy( token1, token );
        return ;
      } // if
      
      scanf( "%c", &ch ); // 將(")讀入   
      ++gcolumn;       
      token[0] = ch;
      ++loc;   // token的第一個位置已有字元)
      if ( cin.peek() != EOF && cin.peek() != '\n' ) {
        scanf( "%c", &ch );
        ++gcolumn;
        for ( findDQ = false ; !findDQ ; ) {  
          if ( ch == '\"' && token[loc] != '\\' ) {  // 2nd DQ
            gItoken = false ;
            findDQ = true ;
            token[++loc] = '\"';
            token[++loc] = '\0';   // +1  for  making ( " ) into string )
            strcpy( token1, token );
            return;
          }  // if
          else                      // the character in string
            token[++loc] = ch ;
          if ( cin.peek() == EOF && !findDQ ) {  // eof
            strcpy( token1, "" );
            cout << endl << "> ERROR (no more input) : END-OF-FILE encountered" ;
            gArriveEOF = true;
            return;
          } // if
          else if ( cin.peek() == '\n' && !findDQ ) {      // end of line
            gerror = true;
            cout << endl << "> ERROR (no closing quote) : END-OF-LINE encountered at ";
            cout << "Line " << gline << " Column " << gcolumn+1 << endl; 
            strcpy( token1, "" );   // Error ""不能分行 )
            return;
          } // else if
          else             // scanf next character
            scanf( "%c", &ch );
          ++gcolumn;
        } // for
      
        if ( !findDQ ) {
          cout << endl << "> ERROR (no closing quote) : END-OF-LINE encountered at ";
          cout << "Line " << gline << " Column " << gcolumn << endl ; 
          strcpy( token, "" );   // Error ""不能分行 )
          strcpy( token1, token );
          gerror = true ;    // 此處為end of line )
          return;
        } // if 	
        else
          strcpy( token1, token );
        return;
      } // if
      else {                                    // EOF )
        strcpy( token1, "" );
        if ( cin.peek() == EOF ) {
          cout << endl << "> ERROR (no more input) : END-OF-FILE encountered" ;
          gArriveEOF = true;
        } // if
        else if ( cin.peek() == '\n' ) {
          cout << endl << "> ERROR (no closing quote) : END-OF-LINE encountered at ";
          cout << "Line " << gline << " Column " << gcolumn+1 << endl;
          gerror = true;
        } // else if

        return;
      } // else
    } // if '\"' )
    else if ( peekch == '(' ) {           // 處理左括弧) 
      if ( gItoken ) {                   // 括弧前的token尚未處理時進入) 
        gItoken = false;
        token[++loc] = '\0';
        strcpy( token1, token );
        return ;
      } // if
      
      scanf( "%c", &ch );
      ++gcolumn;
      if ( cin.peek() != EOF ) 
        peekch = cin.peek();
      while ( cin.peek() != EOF && ( peekch == ' ' || peekch == '\n' ) )  {    // 處理空白  ex: '(  )'之間的空白 )
        if ( peekch == ' ' )
          ++gcolumn;
        else if ( peekch == '\n' ) {
          ++gline;
          gcolumn = 0;
        } // else if

        scanf( "%c", &ch );
        if ( cin.peek() != EOF ) 
          peekch = cin.peek();
      } // while
          
      if ( peekch == ')' ) {
        scanf( "%c", &ch );
        strcpy( token, "()" );
      } // if
      else 
        strcpy( token, "(" );
      strcpy( token1, token ) ;
      return ;
    } // else if '('
    else if ( peekch ==  ')' ) {                        // 處理右括弧) 
      if ( gItoken ) {                                // 括弧前的token尚未處理時進入) 
        gItoken = false;
        token[++loc] = '\0';
        strcpy( token1, token );
        return ;
      } // if
      // check grammer) 
      scanf( "%c", &ch );
      ++gcolumn;
      strcpy( token, ")" );
      strcpy( token1, token );
      return ;     
    } // else if ')' )
    else if ( peekch == ' ' ) {                          // 處理空格) 
      if ( gItoken ) {
        gItoken = false;
        token[++loc] = '\0';
        strcpy( token1, token );
        return ;
      } // if

      scanf( "%c", &ch );
      ++gcolumn;
    } // else if ' '  )
    else if ( peekch ==  '\n' ) {                          // 處理空格) 
      if ( gItoken ) {
        gItoken = false;
        token[++loc] = '\0';
        strcpy( token1, token );
        return ;
      } // if
      
      scanf( "%c", &ch );
      ++gline;
      gcolumn = 0;
    } // else if
    else if ( peekch ==  ';' )  {                         // 處理空格) 
      if ( gItoken ) {
        gItoken = false;
        token[++loc] = '\0';
        strcpy( token1, token );
        return;
      } // if
      
      peekch = cin.peek();  
      for ( char cleaner = '0' ; peekch != '\n' && peekch != EOF ; peekch = cin.peek() ) {  // 讀至行結尾 ) 
        scanf( "%c", &ch );
      } // for 

      gcolumn = 0;
    } // else if
    else if ( peekch == '\'' ) {
      if ( gItoken ) {              // ' 前的token尚未處理時進入) 
        gItoken = false;
        token[++loc] = '\0';
        strcpy( token1, token ) ;
        return;
      } // if
      
      scanf( "%c", &ch );
      ++gcolumn;
      strcpy( token, "'" ) ;
      strcpy( token1, token );
      return;
    } // else if '\'' )
    else {
      if ( !gItoken ) {
        loc = -1;
        gItoken = true;
      } // if
      
      scanf( "%c", &ch );
      ++gcolumn;
      token[++loc] = ch;
    } // else 
  } // while


  strcpy( token1, "" );
  cout << endl << "> ERROR (no more input) : END-OF-FILE encountered" ;
  gArriveEOF = true;
  return;
} // GetNextToken()



void Type( SStr token, SStr type ) {
  int smalldot = 0;
  bool isnum = true;
  bool hasnum = false;
  int i = 0;
  if ( strcmp( token, "(" ) == 0 ) {
    strcpy( type, "LEFT-PAREN" );
    return; 
  } // if
  else if ( strcmp( token, ")" ) == 0 ) {
    strcpy( type, "RIGHT-PAREN" );
    return;
  } // else if
  else if ( strcmp( token, "'" ) == 0 ) {
    strcpy( type, "QUOTE" );
    return;
  } // else if
  else if ( token[0] == '"' ) {  // Deal String & string's escaoe)
    strcpy( type, "STRING" );
    for ( i = 1 ; i < strlen( token ) ; ++i ) {
      if ( token[i] == '\\' ) {
        if ( i+1 < strlen( token ) && token[i+1] == 'n' ) {
          token[i] = '\n';
          Movetofront( token, i+1 );
        } // if
        else if ( i+1 < strlen( token ) && token[i+1] == 't' ) {
          token[i] = '\t';
          Movetofront( token, i+1 );
        } // else if
        else if ( i+1 < strlen( token ) && token[i+1] == '\\' ) {
          token[i] = '\\';
          Movetofront( token, i+1 );
        } // else if
        else if ( i+1 < strlen( token ) && token[i+1] == '"' ) {
          token[i] = '"';
          Movetofront( token, i+1 );
        } // else if          
      } // if
    } // for

    return;
  } // else if
  else if ( strcmp( token, "." ) == 0 ) {
    strcpy( type, "DOT" );
    return;
  } // else if
  else if ( strcmp( token, "()" ) == 0 || strcmp( token, "nil" ) == 0 || strcmp( token, "#f" ) == 0 ) {
    strcpy( type, "NIL" );
    return;
  } // else if
  else if ( strcmp( token, "t" ) == 0 || strcmp( token, "#t" ) == 0 ) {
    strcpy( type, "T" );
    return;
  } // else if
  else if ( token[0] == '+' || token[0] == '-' || isdigit( token[0] ) || token[0] == '.' ) { 
    if ( token[0] == '.' )
      ++smalldot;

    if ( token[0] == '+' || token[0] == '-' ||  token[0] == '.' )  // 檢查第一位) 
      ++i;

    int length = strlen( token );
    for ( ;  smalldot < 2 && i < length && isnum ;  i++ ) {
      if ( token[i] == '.' )              // 計算小數點個數) 
        ++smalldot;
      else if ( !isdigit( token[i] ) )
        isnum = false;              // 檢查個個字元是否為數字) 
        
      if ( isdigit( token[i] ) ) 
        hasnum = true;    
    }  // for

    if ( !isnum || !hasnum ) {
      strcpy( type, "SYMBOL" );
      return; 
    } // if 
    else if ( smalldot == 0 ) {
      strcpy( type, "INT" );
      if ( token[0] == '+' )
        Movetofront( token, 0 ) ;
      return;
    } // else if 
    else if ( smalldot == 1 ) {
      strcpy( type, "FLOAT" );
      return ;
    } // else if
    else if ( smalldot > 1 ) {
      strcpy( type, "SYMBOL" );
      return ;
    } // else if
    else {
      strcpy( type, "SYMBOL" );
      return;
    } // else      
  } // if 	
  else if ( strcmp( token, "" ) == 0 ) {
    strcpy( type, "ERROR" ) ;
    return ;
  } // else if
  else {
    strcpy( type, "SYMBOL" ) ;
    return ;
  } // else
}  // Type()




bool IsAtom( SStr type ) {
  if ( strcmp( type, "STRING" ) == 0 || strcmp( type, "INT" ) == 0 || strcmp( type, "FLOAT" ) == 0  
       || strcmp( type, "SYMBOL" ) == 0 || strcmp( type, "NIL" ) == 0 || strcmp( type, "T" ) == 0 )  
    return true;
  else 
    return false;
} // IsAtom()

void Addwait( NeedRP * head ) {                // 添加右括號等待區+1 )
  NeedRP *runner = head;
  for ( ; runner != NULL ; runner = runner->next ) {
    ++runner->wait;
  }  // for	
} // Addwait()

void NewwaitRP() {  // 新增需要加")"之處 )
  NeedRP * runner = new NeedRP() ;
  int temp;
  int compareNum = 1;
  Addwait( gNeedRP ) ;  
  if ( gNeedRP != NULL ) {
    runner->wait = compareNum;
    runner->next = gNeedRP;
    gNeedRP = runner; 
  } // if
  else {                          // head為NULL時 )
    gNeedRP = new NeedRP();
    gNeedRP->next = NULL;
    gNeedRP->wait = compareNum;
  }  // else
} // NewwaitRP()

void Subwait( NeedRp head ) {                // 添加右括號等待區+1 ) 
  NeedRP * runner = head;
  for ( ; runner != NULL ; runner = runner->next ) {
    --runner->wait;
  }  // for		
} // Subwait()

bool ChwaitRP( NeedRp head ) {   // check waiting Right-Paren )
  NeedRP * runner = head;
  if ( head != NULL && head->wait <= 0 )
    return true;
  else 
    return false;  
} // ChwaitRP()

void DealArriveRP() {
  if ( ChwaitRP( gNeedRP ) ) {
    cin.putback( ')' );              // 右括號到了該放之處，因此放入輸入端)
    --gcolumn;
    gNeedRP = gNeedRP->next;
  } // if
} // DealArriveRP()

bool TypeNlasttype( SStr type ) {   // 前token 與 nowtoken 之間是否需要  ".("   ) 
  if (  strcmp( glasttype, "RIGHT-PAREN" ) == 0 ) {
    if ( strcmp( type, "QUOTE" ) == 0 || IsAtom( type ) || strcmp( type, "LEFT-PAREN" ) == 0 )  {
      // add '(' to right child tree   
      return true;
    } // if		   
  } // if 
  else if ( IsAtom( glasttype ) ) {
    if ( strcmp( type, "QUOTE" ) == 0 || IsAtom( type ) || strcmp( type, "LEFT-PAREN" ) == 0 )  {
      return true;
    } // if	
  }  // else if
    
  return false;
} // TypeNlasttype()

void NewTnode( BTree &head, BTree &Bnode, bool buildleft, SStr token, SStr type ) { 
  // add anew node of tree ) 
  if ( head == NULL ) {
    head = new STree();
    strcpy( head->token, token );
    strcpy( head->type, type ) ;
    strcpy( head->address, "" );
    head->hasquote = false;
    head->left = NULL;
    head->right = NULL;
    Bnode = head;
  } // if
  else {
    BTree newnode = new STree();
    strcpy( newnode->token, token );
    strcpy( newnode->type, type ) ;
    strcpy( newnode->address, "" ); 
    newnode->hasquote = false;
    newnode->left = NULL;
    newnode->right = NULL;
    if ( buildleft )
      Bnode->left = newnode;
    else
      Bnode->right = newnode; 
  } // else
} // NewTnode()

void Printspace( int spacenum ) {
  while ( spacenum > 0 ) {
    cout << ' ';
    --spacenum;
  } // while
} // Printspace()

void Preorder( BTree head, bool isrightchild, int spacenum ) {              // gprintspace 為空白 ) 
  int spacenum1 = spacenum; 
  if ( head != NULL ) {
    if ( strcmp( head->token, "(" ) == 0 ) {                  // 處理 括弧token) 
      if ( !isrightchild ) {
        if ( strcmp( glasttype, "LEFT-PAREN" ) )          // 如果上一個type不為left-paren ) 
          Printspace( spacenum );
        cout << "( ";
        if ( strcmp( head->type, "QUOTE" ) == 0 ) {
          cout << "quote" << endl ;
          strcpy( glasttype, "QUOTE" ) ;
        } // if
        else 
          strcpy( glasttype, "LEFT-PAREN" ) ;     
        spacenum1 = spacenum + 2;
      } // if
    } // if 
    else {                                                 // 處理 非括弧token ) 
      if ( !isrightchild ) {                             // 左子樹的情形 )
        if ( strcmp( glasttype, "LEFT-PAREN" ) )
          Printspace( spacenum );
        if ( strcmp( head->type, "FLOAT" ) == 0 ) 
          printf( "%.3f\n", atof( head->token ) );
        else if ( strcmp( head->type, "FLOAT" ) == 0 ) 
          printf( "%d\n", atoi( head->token ) );
        else if ( strcmp( head->type, "NIL" ) == 0 )
          cout << "nil\n" ; 
        else if ( strcmp( head->type, "T" ) == 0 ) 
          cout << "#t\n";
        else 
          cout << head->token << endl;
          
      }  // if
      else {                                           // 右子樹的情形下) 
        
        if ( strcmp( head->type, "FLOAT" ) == 0 ) {
          Printspace( spacenum );
          cout << "." << endl;
          Printspace( spacenum ); 
          printf( "%.3f\n", atof( head->token ) );
        } // if 
        else if ( strcmp( head->type, "NIL" ) == 0 ) {
        }  // else if 
        else if ( strcmp( head->type, "T" ) == 0 ) {
          Printspace( spacenum );
          cout << "." << endl;
          Printspace( spacenum ); 
          cout << "#t\n";
        } // else if 
        else {
          Printspace( spacenum );
          cout << "." << endl;
          Printspace( spacenum ); 
          cout << head->token << endl;
        } // else 
      } // else
      
      strcpy( glasttype, "" ) ; 
    } // else

    Preorder( head->left, false, spacenum1 );
    Preorder( head->right, true, spacenum1 );
    if ( ( strcmp( head->type, "LEFT-PAREN" ) == 0 || strcmp( head->type, "QUOTE" ) == 0 ) 
         && !isrightchild  ) {
      Printspace( spacenum );
      cout << ")" << endl;
    } // if
  } // if   head != NULL )
} // Preorder()

void Quoter( BTree &tree ) {
  if ( tree != NULL ) {
    tree->hasquote = true;
    Quoter( tree->left );
    Quoter( tree->right );
  } // if
} // Quoter()
/*
void CoutWaitingRP() {
  NeedRp runner = gNeedRP;
  cout << " WaitingRP : ";
  while ( runner != NULL ) {
    cout << runner->wait;
    runner = runner->next;
  } // while

  cout << endl;
} // CoutWaitingRP()
*/

bool IsSexp( BTree &head, BTree Bnode, SStr token, SStr type, bool hrp, bool hdot ) {
  BTree pointer = Bnode;
  if ( strcmp( type, "LEFT-PAREN" ) == 0 || strcmp( type, "QUOTE" ) == 0 || TypeNlasttype( type ) )  {
    SStr token1;
    SStr type1;
    if ( TypeNlasttype( type ) ) {  // 前後token之間需加 ".(" 時)
      PutbackToken( token );
      strcpy( type, "LEFT-PAREN" );
      strcpy( token, "(" );
      NewTnode( head, pointer, false, token, type );  // false 表示加入右子樹 ) 
      NewwaitRP();
      pointer = pointer->right;
    } // if
    else {
      strcpy( token, "(" );
      if ( head == NULL ) {
        NewTnode( head, Bnode, false, token, type );  // 此處因為head為NULL 故填入false or true無相關 新增node到head)
        pointer = Bnode;
      } // if 
      else if ( strcmp( glasttype, "DOT" ) == 0 ) {
        NewTnode( head, Bnode, false, token, type );  // false 表示加入右子樹 )
        pointer = Bnode->right;
      } // if
      else {
        NewTnode( head, Bnode, true, token, type );  // true 表示加入左子樹 )
        pointer = Bnode->left; 
      } // else

      Addwait( gNeedRP );      //  ++等待加右括號位置,因為碰到 ( or ' ) 
    } // else
    

    strcpy( glasttype, type );    // lasttype = type( LEFT-PAREN or ' )為下一type做準備 ) 
    GetNextToken( token1 );
    Type( token1, type1 );


    if ( !gArriveEOF && !gerror && IsSexp( head, pointer, token1, type1, false, false ) ) { 
      // ( "<S-EXP>" {<S-EXP>} (.<S-EXP> ) ) 
      if ( strcmp( type, "QUOTE" ) != 0 ) {
        GetNextToken( token1 );
        Type( token1, type1 );
        while ( !gArriveEOF && !gerror && IsSexp( head, pointer, token1, type1, true, true ) ) { 
          // ( <S-EXP> "{<S-EXP>}" (.<S-EXP> ) )
          GetNextToken( token1 );
          Type( token1, type1 );
        } // while

        if ( !gArriveEOF && !gerror && strcmp( type1, "DOT" ) == 0 ) {
          GetNextToken( token1 );
          Type( token1, type1 );
          strcpy( glasttype, "DOT" ) ;
          if ( !gArriveEOF && !gerror && IsSexp( head, pointer, token1, type1, false, false ) )  {
            // ( <S-EXP> {<S-EXP>} ."( <S-EXP> )" )
            GetNextToken( token1 );
            Type( token1, type1 );
            if ( !gArriveEOF && !gerror && strcmp( type1, "RIGHT-PAREN" ) == 0 ) {
              Subwait( gNeedRP );      
              DealArriveRP();
              strcpy( glasttype, "RIGHT-PAREN" ) ;
              return true;
            } // if
            else {
              if ( !gArriveEOF && !gerror ) {
                gerror = true;
                cout << endl << "> ERROR (unexpected token) : ')' expected when token at ";
                cout << "Line " << gline <<  " Column " << gcolumn-strlen( token1 ) + 1 
                << " is >>" << token1 << "<<" << endl ;
                // ERROR  缺少 " ) " ) 
              } // if

              return false;
            } // else 
          } // if
          else
            return false;
        } // if
      }  // if
      else {                                                      // 處理quote) 
        cin.putback( ')' );
        GetNextToken( token1 );
        Type( token1, type1 );
      } // else

      if ( !gArriveEOF && !gerror && strcmp( type1, "RIGHT-PAREN" ) == 0 ) {

        Subwait( gNeedRP );
        DealArriveRP();
        strcpy( glasttype, "RIGHT-PAREN" );       // atom- lasttype= type(RIGHT-PAREN)為下一type做準備 ) 
        return true;
      } // if
      else {
        if ( !gArriveEOF && !gerror ) {
          gerror = true;
          cout << endl << "> ERROR (unexpected token) : ')' expected when token at ";
          cout << "Line " << gline <<  " Column " << gcolumn-strlen( token1 ) + 1 
          << " is >>" << token1 << "<<" << endl ;
          // ERROR  : // ( <S-EXP> {<S-EXP>} (.<S-EXP> ) ")"   雙引號內部有誤  應為atom or ( or  ')
        } // if

        return false;
      } // else
    } // if
    else 
      // ERROR : // ( "<S-EXP>" {<S-EXP>} (.<S-EXP> ) ) 雙引號內部有誤  應為atom or ( or  ' ) 
      return false;
  }  // if
  else if ( IsAtom( type ) )  {

    if ( strcmp( glasttype, "DOT" ) == 0 ) {        // 假如上一個token為"." ) 
      NewTnode( head, Bnode, false, token, type );  // false 表示加入右子樹 )
    } // if
    else  {
      NewTnode( head, Bnode, true, token, type );  // false 表示加入左子樹 )  
    } // else

    strcpy( glasttype, type );    // atom- lasttype= type(ATOM)為下一type做準備 )  

    return true;
  } // else if
  else {
    // ERROR : // ( "<S-EXP>" {<S-EXP>} (.<S-EXP> ) ) 雙引號內部有誤  應為atom or ( or  ' )
    if ( hdot && strcmp( "DOT", type ) == 0 ) {                     // 允許 "." 出現之處)
    } // if
    else if ( hrp && strcmp( "RIGHT-PAREN", type ) == 0 ) {        // 允許 ")"  出現之處 )
    } // else if
    else if ( gArriveEOF || gerror ) {
    } // else if
    else {    // here )
      gerror = true;
      cout << endl << "> ERROR (unexpected token) : atom or '(' expected when token at " ;
      cout << "Line " << gline <<  " Column " << gcolumn-strlen( token ) + 1 
      << " is >>" << token << "<<" << endl ;
    } // else

    return false;
  } // else

  return false;
}  // IsSexp()




void CleanTree( BTree &head ) {
  if ( head != NULL ) {
    CleanTree( head->left );
    CleanTree( head->right );
    strcpy( head->type, "" );
    strcpy( head->token, "" );
    strcpy( head->address, "" );
    delete head;
    head = NULL;
  } // if
} // CleanTree()

bool Checkexit( BTree head ) {
  if ( head != NULL && strcmp( head->type,  "LEFT-PAREN" ) == 0 ) {
    if ( head->left != NULL && strcmp( head->left->token, "exit" )  == 0 && head->left->left == NULL )  {
      if ( head->right == NULL || ( head->right != NULL && strcmp( head->right->type, "NIL" ) == 0 ) )
        return true;   
    } // if
  } // if

  return false;  
} // Checkexit()

void CleanWaitingRP( NeedRp &head ) {
  NeedRp temp;
  if ( head != NULL ) {
    temp = head->next;
    head = NULL;
    delete head;
    CleanWaitingRP( temp ); 
  } // if
} // CleanWaitingRP()

// ******************* Proj2 ************************

void Fota( SStr &num1, float num2 ) {
  sprintf( num1, "%f", num2 );
} // Fota()

bool IsatomT( BTree head ) {        // Seqnum : sequence 數量 )  
  if ( head->left != NULL && IsAtom( head->left->type ) 
       && ( head->right == NULL || ( head->right != NULL && strcmp( head->right->type, "NIL" ) == 0 ) ) )
    return true;
  else 
    return false;    
} // IsatomT()

bool Isdottedpair( BTree head ) {  // dotted pair ex: (1 . 2 )  right child must be atom. 
  // bcz  "( 1 . ( 2 3 )" = "( 1 2 3 )" 
  if ( head->left != NULL && head->right != NULL )   // 此處允許nil  ) 
    return true;
  return false; 
} // Isdottedpair()

/*
bool Islist( BTree head ) {  // use in list? )
  BTree runner = head;
  if ( head->left != NULL && 
       ( ( head->right != NULL && strcmp( head->right->type, "NIL" ) == 0 ) || head->right == NULL ) ) {
    if ( head->left->right != NULL && strcmp( head->left->type, "NIL" ) != 0 )
      return true;
    else
      return false;
  }  // if

  return false;
} // Islist()
*/

bool Islist2( BTree head ) {  // use for checking error )
  BTree runner = head;
  while ( runner != NULL && strcmp( runner->token, "(" ) == 0 )
    runner = runner->right;
  if ( runner == NULL || strcmp( runner->type, "NIL" ) == 0 )
    return true;

  return false;
} // Islist2()

bool Ispair( BTree head ) {
  if ( strcmp( head->left->type, "LEFT-PAREN" ) == 0  )
    return true;
  return false;
} // Ispair()

bool Isnull( BTree head ) {
  if ( head->left != NULL && strcmp( head->left->type, "NIL" ) == 0 
       && ( head->right == NULL || ( head->right != NULL && strcmp( head->right->type, "NIL" ) == 0 ) ) ) 
    return true;
  return false;
}  // Isnull()

bool IsInteger( BTree head ) {
  if ( head->left != NULL && strcmp( head->left->type, "INT" ) == 0 
       && ( head->right == NULL || ( head->right != NULL && strcmp( head->right->type, "NIL" ) == 0 ) ) ) 
    return true;
  return false;
} // IsInteger()

bool Isstring( BTree head ) {
  if ( head->left != NULL && strcmp( head->left->type, "STRING" ) == 0 
       && ( head->right == NULL || ( head->right != NULL && strcmp( head->right->type, "NIL" ) == 0 ) ) ) 
    return true;
  return false;
} // Isstring()

bool Issymbol( BTree head ) {
  if ( head->left != NULL && strcmp( head->left->type, "SYMBOL" ) == 0 
       && ( head->right == NULL || ( head->right != NULL && strcmp( head->right->type, "NIL" ) == 0 ) ) ) 
    return true;
  return false;
} // Issymbol()

bool Isboolean( BTree head ) {
  if ( head->left != NULL 
       && ( strcmp( head->left->type, "T" ) == 0 || strcmp( head->left->type, "NIL" ) == 0 )
       && ( head->right == NULL || ( head->right != NULL && strcmp( head->right->type, "NIL" ) == 0 ) ) ) 
    return true;
  return false;
} // Isboolean()


bool Isnumber( BTree head ) {
  if ( head->left != NULL 
       && ( strcmp( head->left->type, "INT" ) == 0 || strcmp( head->left->type, "FLOAT" ) == 0 )
       && ( head->right == NULL || ( head->right != NULL && strcmp( head->right->type, "NIL" ) == 0 ) ) ) 
    return true;
  return false;
} // Isnumber()

bool IsInstruction( SStr token ) {

  if ( strcmp( token, "quote" ) == 0 ) 
    return true;
  else if ( strcmp( token, "cons" ) == 0 )
    return true;
  else if ( strcmp( token, "define" ) == 0 ) 
    return true;
  else if ( strcmp( token, "list" ) == 0 ) 
    return true;
  else if ( strcmp( token, "car" ) == 0 ) 
    return true;
  else if ( strcmp( token, "cdr" ) == 0 ) 
    return true;
  else if ( strcmp( token, "atom?" ) == 0 ) 
    return true;
  else if ( strcmp( token, "pair?" ) == 0 ) 
    return true;
  else if ( strcmp( token, "list?" ) == 0 ) 
    return true;
  else if ( strcmp( token, "null?" ) == 0 ) 
    return true;
  else if ( strcmp( token, "integer?" ) == 0 ) 
    return true;
  else if ( strcmp( token, "real?" ) == 0 ) 
    return true;
  else if ( strcmp( token, "number?" ) == 0 )
    return true;
  else if ( strcmp( token, "string?" ) == 0 ) 
    return true;
  else if ( strcmp( token, "boolean?" ) == 0 ) 
    return true;
  else if ( strcmp( token, "symbol?" ) == 0 ) 
    return true;
  else if ( strcmp( token, "+" ) == 0 ) 
    return true;
  else if ( strcmp( token, "-" ) == 0 ) 
    return true;
  else if ( strcmp( token, "*" ) == 0 ) 
    return true;
  else if ( strcmp( token, "/" ) == 0 ) 
    return true;
  else if ( strcmp( token, "not" ) == 0 ) 
    return true;
  else if ( strcmp( token, "and" ) == 0 ) 
    return true;
  else if ( strcmp( token, "or" ) == 0 ) 
    return true;
  else if ( strcmp( token, ">" ) == 0 ) 
    return true;
  else if ( strcmp( token, "<" ) == 0 ) 
    return true;
  else if ( strcmp( token, ">=" ) == 0 ) 
    return true;
  else if ( strcmp( token, "<=" ) == 0 ) 
    return true;
  else if ( strcmp( token, "=" ) == 0 ) 
    return true;
  else if ( strcmp( token, "string-append" ) == 0 ) 
    return true;
  else if ( strcmp( token, "string>?" ) == 0 ) 
    return true;
  else if ( strcmp( token, "string<?" ) == 0 ) 
    return true;
  else if ( strcmp( token, "string=?" ) == 0 ) 
    return true;
  else if ( strcmp( token, "eqv?" ) == 0 ) 
    return true;
  else if ( strcmp( token, "equal?" ) == 0 ) 
    return true;
  else if ( strcmp( token, "begin" ) == 0 ) 
    return true;
  else if ( strcmp( token, "if" ) == 0 )
    return true;
  else if ( strcmp( token, "cond" ) == 0 ) 
    return true;
  else if ( strcmp( token, "clean-environment" ) == 0 ) 
    return true;
  else if ( strcmp( token, "exit" ) == 0 ) 
    return true;
  else if ( strcmp( token, "let" ) == 0 ) 
    return true;
  else if ( strcmp( token, "lambda" ) == 0 ) 
    return true;


  return false;
} // IsInstruction()

void CopyTree( BTree head, BTree &head2 ) {
  if ( head != NULL ) {
    head2 = NULL;
    head2 = new STree() ;
    strcpy( head2->token, head->token ) ;
    strcpy( head2->type, head->type ) ;
    strcpy( head2->address, head->address ) ;
    head2->hasquote = head->hasquote;
    CopyTree( head->left, head2->left );
    CopyTree( head->right, head2->right );    
  } // if
  else
    head2 = NULL;
}  // CopyTree()

void Address_plus( SStr name, BTree head ) {
  if ( head != NULL ) {
    if ( strcmp( head->address, "" ) == 0 )
      strcpy( head->address, name );
    Address_plus( name, head->left );
    Address_plus( name, head->right );
  } // if
} // Address_plus()

void NewdefinedArea( AreavariableList &deList, DefinitionList node_betweenArea ) { 
  // new an area to put variable 
  AreavariableList new1 = new Areavariable();
  new1->linker = node_betweenArea;
  new1->list = NULL;
  new1->next = deList;
  deList = new1;
} // NewdefinedArea()

void NewDefinition( AreavariableList &gdeList, SStr name, BTree head, bool isfunction ) {
  if ( gdeList->list == NULL ) {
    gdeList->list = new DefinitionNode();
    strcpy( gdeList->list->name, name );
    Address_plus( name, head ); 
    CopyTree( head, gdeList->list->head );
    gdeList->list->isfunction = isfunction;
    gdeList->list->next = NULL;
  } // if
  else {
    DefinitionList linker = new DefinitionNode();
    strcpy( linker->name, name );
    Address_plus( name, head ); 
    CopyTree( head, linker->head );
    linker->isfunction = isfunction;
    linker->next = gdeList->list;
    gdeList->list = linker;  
  } // else
} // NewDefinition()

void InstructionDefinition() {
  BTree instruhead = new STree();
  strcpy( instruhead->address, "" );
  SStr token;
  instruhead->left = NULL;
  instruhead->right = NULL;

  strcpy( token, "quote" );  
  strcpy( instruhead->token, "#<procedure quote>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "cons" );
  strcpy( instruhead->token, "#<procedure cons>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "define" );
  strcpy( instruhead->token, "#<procedure define>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "list" );
  strcpy( instruhead->token, "#<procedure list>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "car" );
  strcpy( instruhead->token, "#<procedure car>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "cdr" );
  strcpy( instruhead->token, "#<procedure cdr>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "atom?" );
  strcpy( instruhead->token, "#<procedure atom?>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "pair?" );
  strcpy( instruhead->token, "#<procedure pair?>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "list?" );
  strcpy( instruhead->token, "#<procedure list?>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "null?" );
  strcpy( instruhead->token, "#<procedure null?>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "integer?" );
  strcpy( instruhead->token, "#<procedure integer?>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "real?" );
  strcpy( instruhead->token, "#<procedure real?>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "number?" );
  strcpy( instruhead->token, "#<procedure number?>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "string?" );
  strcpy( instruhead->token, "#<procedure string?>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "boolean?" );
  strcpy( instruhead->token, "#<procedure boolean?>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "symbol?" );
  strcpy( instruhead->token, "#<procedure symbol?>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "+" );
  strcpy( instruhead->token, "#<procedure +>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "-" );
  strcpy( instruhead->token, "#<procedure ->" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "*" );
  strcpy( instruhead->token, "#<procedure *>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "/" );
  strcpy( instruhead->token, "#<procedure />" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "not" );
  strcpy( instruhead->token, "#<procedure not>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "and" );
  strcpy( instruhead->token, "#<procedure and>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "or" );
  strcpy( instruhead->token, "#<procedure or>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, ">" );
  strcpy( instruhead->token, "#<procedure >>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "<" );
  strcpy( instruhead->token, "#<procedure <>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, ">=" );
  strcpy( instruhead->token, "#<procedure >=>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "<=" );
  strcpy( instruhead->token, "#<procedure <=>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "=" );
  strcpy( instruhead->token, "#<procedure =>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "string-append" );
  strcpy( instruhead->token, "#<procedure string-append>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "string>?" );
  strcpy( instruhead->token, "#<procedure string>?>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "string<?" );
  strcpy( instruhead->token, "#<procedure string<?>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "string=?" );
  strcpy( instruhead->token, "#<procedure string=?>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "eqv?" );
  strcpy( instruhead->token, "#<procedure eqv?>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "equal?" );
  strcpy( instruhead->token, "#<procedure equal?>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "begin" );
  strcpy( instruhead->token, "#<procedure begin>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "if" );
  strcpy( instruhead->token, "#<procedure if>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "cond" );
  strcpy( instruhead->token, "#<procedure cond>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "clean-environment" );
  strcpy( instruhead->token, "#<procedure clean-environment>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "exit" );
  strcpy( instruhead->token, "#<procedure exit>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "let" );
  strcpy( instruhead->token, "#<procedure let>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

  strcpy( token, "lambda" );
  strcpy( instruhead->token, "#<procedure lambda>" );
  strcpy( instruhead->type, "INSTRUCTION" );
  NewDefinition( gdeList, token, instruhead, false );

} // InstructionDefinition()

int AtomNum( BTree head ) { // 以 " .( " +1("開頭") 之數量為基準 )  
  int num = 0;
  BTree runner = head; 
  for ( ; runner != NULL && strcmp( runner->type, "NIL" ) != 0 ; runner = runner->right ) {
    if ( strcmp( runner->type, "LEFT-PAREN" ) == 0 ) {
      ++num;
    } // if
  } // for

  return num;
} // AtomNum()

bool CheckifinDeList_S( SStr token, BTree &head, AreavariableList deList, DefinitionList nownode ) { 
  // 找出symbol代表的樹 )
  AreavariableList runarea = deList;
  DefinitionList runner = nownode;
  DefinitionList tempnext = nownode;
  // cout << "nownode->name" << nownode->name << endl;
  // system("pause");
  for ( ; runarea != NULL ; runarea = runarea->next ) {
    if ( runarea != deList )
      nownode = tempnext;
    for ( runner = nownode ; runner != NULL ; runner = runner->next ) {
      // 在definitionList中尋找與token相同名稱的 has defined symbol )
      if ( strcmp( runner->name, token ) == 0 ) {
        if ( strcmp( runner->head->type, "SYMBOL" ) == 0 && !runner->head->hasquote ) { 
          // 假如某symbol的宣告內容為symbol ) 
          AreavariableList maygonext = runarea;
          if ( runarea->next != NULL ) { // if a variable we found is a local variable
            deList = runarea->next;
            nownode = runarea->linker;
          } // if
          else {
            deList = runarea;
            nownode = runner->next;
          } // else

          if ( CheckifinDeList_S( runner->head->token, head, deList, nownode ) )
            return true;
          else
            return false;
        } // if
        else  {
          CopyTree( runner->head, head );
          nownode = runner; // bcz myabe we need to caculate the head of the origin symbol
          deList = runarea; // so we need to return its address
        } // else

        return true;
      } // if
    } // for

    if ( runarea->next != NULL )
      tempnext = runarea->linker;
  } // for

  return false;
} // CheckifinDeList_S()

bool CheckifinDeList_S1( SStr token, BTree &head, AreavariableList deList, DefinitionList nownode ) { 
  // 找出symbol代表的樹  gdeList find the same name from gdeList && 
  // if the head of the symbol we found is another symbol, find the symbol from gdeList2 )
  AreavariableList runarea = deList;
  DefinitionList runner = nownode;
  DefinitionList tempnext = nownode;
  // cout << "nownode->name" << nownode->name << endl;
  // system("pause"); 
  for ( ; runarea != NULL ; runarea = runarea->next ) {
    if ( runarea != deList )
      nownode = tempnext;
    for ( runner = nownode ; runner != NULL ; runner = runner->next ) {    
      // 在definitionList中尋找與token相同名稱的 has defined symbol ) 
      if ( strcmp( runner->name, token ) == 0 ) {
        if ( strcmp( runner->head->type, "SYMBOL" ) == 0 && !runner->head->hasquote ) { 
          // 假如某symbol的宣告內容為symbol ) 
          AreavariableList maygonext = runarea ;
          if ( runarea->next != NULL ) {
            // if the symbol we found is local variable, & its head is another symbol )
            deList = runarea->next;
            nownode = runarea->linker;
          } // if
          else {   // if the symbol we found is global variable, & its head is another symbol ) 
            deList = runarea;
            nownode = runner->next;
          } // else

          if ( CheckifinDeList_S1( runner->head->token, head, deList, nownode ) ) {
            return true;
          } // if
          else
            return false;
        } // if
        else {
          if ( runner->isfunction ) {
            SStr tokenname = "#<procedure ";
            strcat( tokenname, runner->name );
            strcat( tokenname, ">" );
            strcpy( head->address, runner->name );
            strcpy( head->token, tokenname );
            strcpy( head->type, "PREFUNCTION" );
            CleanTree( head->left );
            CleanTree( head->right );
            nownode = runner; // myabe we need to caculate the head of the origin symbol
            deList = runarea; // so we need to return its address
          } // if
          else {
            CopyTree( runner->head, head );
            nownode = runner; //  myabe we need to caculate the head of the origin symbol
            deList = runarea; // so we need to return its address
          } // else
        } // else

        return true;
      } // if
    } // for

    if ( runarea->next != NULL )
      tempnext = runarea->linker;
  } // for

  return false;
} // CheckifinDeList_S1()

/*
bool CheckifinDeList( SStr token, AreavariableList gdeList, DefinitionList nownode ) { 
  // search the token whether it is in defined symbol list )
  AreavariableList runarea = gdeList;
  DefinitionList runner = nownode;
  cout << "first : " >> endl; 
  for ( ; runarea != NULL ; runarea = runarea->next ) {
    nownode = runarea->list;
    for ( runner = nownode ; runner != NULL ; runner = runner->next ) {    
      // 在definitionList中尋找與token相同名稱的 has defined symbol ) 
      if ( strcmp( runner->name, token ) == 0 ) {
        if ( strcmp( runner->head->type, "SYMBOL" ) == 0  ) { // 假如某symbol的宣告內容為symbol )
          AreavariableList maygonext = runarea ;
          if ( runarea->next != NULL ) {
            maygonext = runarea->next;
            nownode = runarea->next->list;
          } // if
          else
            nownode = runner->next;

          if ( CheckifinDeList( runner->head->token, maygonext, nownode ) )
            return true;
          else
            return false;
        } // if
        else  {
          return true;
        } // else

        return true;
      } // if
    } // for
  } // for

  return false;
} // CheckifinDeList()
*/

bool CheckifisFunctioninDeList( SStr token, AreavariableList deList, DefinitionList nownode ) {
  AreavariableList runarea = deList;
  DefinitionList runner = nownode;
  DefinitionList tempnext = nownode;
  for ( ; runarea != NULL ; runarea = runarea->next ) {
    if ( runarea != deList )
      nownode = tempnext;
    for ( runner = nownode ; runner != NULL ; runner = runner->next ) {
      // 在definitionList中尋找與token相同名稱的 has defined symbol ) 
      if ( strcmp( runner->name, token ) == 0 ) {
        if ( runner->isfunction ) { //  if isfunction )
          return true;
        } // if

      } // if
    } // for
    
    if ( runarea->next != NULL )
      tempnext = runarea->linker;
  } // for

  return false;
} // CheckifisFunctioninDeList()

/*
bool Checkifin1DeList( SStr token ) { 
  // 找出symbol代表的樹  used by the instruction which named define ) 
  DefinitionList runner = gdeList->list;
  for ( runner = gdeList->list ; runner != NULL ; runner = runner->next ) {
      // 在definitionList中尋找與token相同名稱的 has defined symbol ) 
    if ( strcmp( runner->name, token ) == 0 ) {
      if ( strcmp( runner->head->type, "SYMBOL" ) == 0  ) { // 假如某symbol的宣告內容為symbol ) 
        if ( Checkifin1DeList( runner->head->token ) )
          return true;
        else
          return false;
      } // if

      return true;
    } // if

  } // for
  
  return false;
} // Checkifin1DeList()
*/
void ProceduretoIns( BTree head ) {
  BTree instruhead = new STree();
  SStr token;
  instruhead->left = NULL;
  instruhead->right = NULL;

  if ( strcmp( head->token, "#<procedure quote>" ) == 0 ) {
    strcpy( head->token, "quote" );  
    strcpy( head->type, "SYMBOL" );
    return;
  } // if

  if ( strcmp( head->token, "#<procedure cons>" ) == 0 ) {
    strcpy( head->token, "cons" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if

  if ( strcmp( head->token, "#<procedure define>" ) == 0 ) {
    strcpy( head->type, "SYMBOL" );
    strcpy( head->token, "define" );
    return;
  } // if

  if ( strcmp( head->token, "#<procedure list>" ) == 0 ) {
    strcpy( head->token, "list" );
    strcpy( head->type, "SYMBOL" ); 
    return; 
  } // if

  if ( strcmp( head->token, "#<procedure car>" ) == 0 ) {
    strcpy( head->token, "car" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if

  if ( strcmp( head->token, "#<procedure cdr>" ) == 0 ) {
    strcpy( head->token, "cdr" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if

  if ( strcmp( head->token, "#<procedure atom?>" ) == 0 ) {
    strcpy( head->token, "atom?" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure pair?>" ) == 0 ) {
    strcpy( head->token, "pair?" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure list?>" ) == 0 ) {
    strcpy( head->token, "list?" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure null?>" ) == 0 ) {
    strcpy( head->token, "null?" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure integer?>" ) == 0 ) {
    strcpy( head->token, "integer?" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if

  if ( strcmp( head->token, "#<procedure real?>" ) == 0 ) {
    strcpy( head->token, "real?" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure number?>" ) == 0 ) {
    strcpy( head->token, "number?" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure string?>" ) == 0 ) {
    strcpy( head->token, "string?" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure boolean?>" ) == 0 ) {
    strcpy( head->token, "boolean?" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure symbol?>" ) == 0 ) {
    strcpy( head->token, "symbol?" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure +>" ) == 0 ) {
    strcpy( head->token, "+" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure ->" ) == 0 ) {
    strcpy( head->token, "-" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure *>" ) == 0 ) {
    strcpy( head->token, "*" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure />" ) == 0 ) {
    strcpy( head->token, "/" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure not>" ) == 0 ) {
    strcpy( head->token, "not" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure and>" ) == 0 ) {
    strcpy( head->token, "and" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure or>" ) == 0 ) {
    strcpy( head->token, "or" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure >>" ) == 0 ) {
    strcpy( head->token, ">" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure <>" ) == 0 ) {
    strcpy( head->token, "<" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure >=>" ) == 0 ) {
    strcpy( head->token, ">=" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // ifv  

  if ( strcmp( head->token, "#<procedure <=>" ) == 0 ) {
    strcpy( head->token, "<=" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if

  if ( strcmp( head->token, "#<procedure =>" ) == 0 ) {
    strcpy( head->token, "=" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure string-append>" ) == 0 ) {
    strcpy( head->token, "string-append" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure string>?>" ) == 0 ) {
    strcpy( head->token, "string>?" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure string<?>" ) == 0 ) {
    strcpy( head->token, "string<?" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure string=?>" ) == 0 ) {
    strcpy( head->token, "string=?" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure eqv?>" ) == 0 ) {
    strcpy( head->token, "eqv?" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure equal?>" ) == 0 ) {
    strcpy( head->token, "equal?" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure begin>" ) == 0 ) {
    strcpy( head->token, "begin" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure if" ) == 0 ) {
    strcpy( head->token, "if" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure cond>" ) == 0 ) {
    strcpy( head->token, "cond" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if  

  if ( strcmp( head->token, "#<procedure clean-environment>" ) == 0 ) {
    strcpy( head->token, "clean-environment" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if

  if ( strcmp( head->token, "#<procedure exit>" ) == 0 ) {
    strcpy( head->token, "exit" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if

  if ( strcmp( head->token, "#<procedure lambda>" ) == 0 ) {
    strcpy( head->token, "clean-environment" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if

  if ( strcmp( head->token, "#<procedure let>" ) == 0 ) {
    strcpy( head->token, "exit" );
    strcpy( head->type, "SYMBOL" );
    return;
  } // if

} // ProceduretoIns()

/*
bool Rewritedelist( SStr token, BTree head, bool isfunction ) { // 找出symbol代表的樹 )
  DefinitionList runner = gdeList->list;
  while ( runner != NULL ) {    // 在definitionList中尋找與token相同名稱的 has defined symbol ) 
    if ( strcmp( runner->name, token ) == 0 ) {
      CleanTree( runner->head );
      Address_plus( token, head ); 
      CopyTree( head, runner->head );
      runner->isfunction = isfunction;
      return true;
    } // if

    runner = runner->next;
  } // while
  
  return false;
} // Rewritedelist()
*/

void ChecknameinDeList( SStr name, SStr &originname, AreavariableList deList, DefinitionList nownode ) { 
  // 在確定文法符合的情況下才可使用 ) 
  AreavariableList runarea = deList;  // it must not be null bcz instruction )
  DefinitionList runner = nownode;
  DefinitionList tempnext = nownode;
  for ( ; runarea != NULL ; runarea = runarea->next ) {
    if ( runarea != deList )
      nownode = tempnext;
    for ( runner = nownode ; runner != NULL ; runner = runner->next ) { 
      // 在definitionList中尋找與token相同名稱的 has defined symbol )
      if ( strcmp( runner->name, name ) == 0 ) {
        if ( strcmp( runner->head->type, "SYMBOL" ) == 0 && !runner->head->hasquote ) { 
          // 假如某symbol的宣告內容為symbol ) 
          AreavariableList maygonext = runarea ;
          if ( runarea->next != NULL ) { 
            // if the symbol we found is local variable, & its head is another symbol )
            deList = runarea->next;
            nownode = runarea->linker;
          } // if
          else {   // if the symbol we found is global variable, & its head is another symbol ) 
            deList = runarea;
            nownode = runner->next;
          } // else

          ChecknameinDeList( runner->head->token, originname, deList, nownode );
        } // if
        else
          strcpy( originname, runner->name ) ;
        return; 
      } // if

    } // for

    if ( runarea->next != NULL )
      tempnext = runarea->linker;
  } // for
  
  return;

} // ChecknameinDeList()

bool Isequal( BTree head1, BTree head2 ) {
  if ( ( ( head1 != NULL && strcmp( head1->type, "NIL" ) != 0 ) 
         && ( head2 != NULL && strcmp( head2->type, "NIL" ) != 0 ) )
       && strcmp( head1->token, head2->token ) == 0 
       && strcmp( head1->type, head2->type ) == 0 ) {
    if ( Isequal( head1->left, head2->left ) && Isequal( head1->right, head2->right )  )
      return true;
  } // if
  else if ( ( head1 == NULL || ( head1 != NULL && strcmp( head1->type, "NIL" ) == 0 ) )
            && ( head2 == NULL || ( head2 != NULL && strcmp( head2->type, "NIL" ) == 0 ) ) )
    return true ;
  return false;
} // Isequal()

void CleanDeList( DefinitionList &deList1 ) {
  DefinitionList runner = deList1;
  while ( runner != NULL && strcmp( runner->head->type, "INSTRUCTION" ) != 0 ) {
    CleanTree( deList1->head );
    runner = runner->next; 
  } // while
  
  deList1 = runner;
} // CleanDeList()

void DeleteTopAreavariableList( AreavariableList &gdeList ) {
  if ( gdeList->next != NULL ) {
    CleanDeList( gdeList->list );
    gdeList = gdeList->next;
  } // if
} // DeleteTopAreavariableList()

void Clean_environment( AreavariableList &gdeList ) {
  while ( gdeList->next != NULL ) {

    DeleteTopAreavariableList( gdeList );

  } // while

  CleanDeList( gdeList->list ) ;
} // Clean_environment()

bool Iseqv( BTree head1, BTree head2 ) {
  if ( strcmp( head1->type, "SYMBOL" ) == 0 && strcmp( head2->type, "SYMBOL" ) == 0 ) {
    SStr name1, name2;
    ChecknameinDeList( head1->token, name1, gdeList, gdeList->list );
    ChecknameinDeList( head2->token, name2, gdeList, gdeList->list );
    if ( strcmp( name1, name2 ) == 0 ) 
      return true;
    else
      return false;
  } // if
  else if ( strcmp( head1->address, "" ) != 0 && strcmp( head2->address, "" ) != 0 ) {
    SStr originname1, originname2;
    strcpy( originname1, "" );
    strcpy( originname2, "" );
    ChecknameinDeList( head1->address, originname1, gdeList, gdeList->list );
    ChecknameinDeList( head2->address, originname2, gdeList, gdeList->list );
    if ( strcmp( originname1, "" ) != 0 && strcmp( originname2, "" ) != 0 
         && strcmp( originname1, originname2 ) == 0 && Isequal( head1, head2 ) )
      return true;
  } // else if
  else if ( ( strcmp( head1->address, "" ) == 0 && strcmp( head2->address, "" ) == 0 ) && 
            IsAtom( head1->type ) && IsAtom( head2->type ) ) { 
    if ( strcmp( head1->type, "STRING" ) != 0 && strcmp( head2->type, "STRING" ) != 0 )
      return true;
  } // else if 

  return false;
} // Iseqv()
// ********************* 

bool Argumentnum( int mix, int max, BTree head, SStr instructionname, bool needprint ) {
  int actualnum = 0;
  for ( BTree runner = head ; runner != NULL && strcmp( runner->type, "NIL" ) != 0
        && runner->left != NULL ; runner = runner->right, ++actualnum ) {
  } // for

  if ( actualnum >= mix && actualnum <= max ) {
    return true;
  } // if
  
  if ( needprint )
    cout << "ERROR (incorrect number of arguments) : " << instructionname << endl; 
  return false;
} // Argumentnum()

void BeCommendhead( BTree &definehead, SStr name ) {
  CleanTree( definehead );
  definehead = new STree();
  strcpy( definehead->token, "(" );
  strcpy( definehead->type, "LEFT-PAREN" );

  definehead->left = new STree();
  strcpy( definehead->left->token, name );
  strcpy( definehead->left->type, "SYMBOL" );
  definehead->left->left = NULL;
  definehead->left->right = NULL;

  definehead->right = NULL;
} // BeCommendhead()

void NewTemplambda( BTree parameter, BTree statement ) {
  // create the new lambda function
  TemplambdaList new1 = new Templambda();
  if ( gtemplambdaList != NULL ) {
    CopyTree( parameter, new1->parameter );
    CopyTree( statement, new1->statement );
    sprintf( new1->name, "%d", gfunctionnum );
    ++gfunctionnum;
    new1->next = gtemplambdaList;
    gtemplambdaList = new1;
  } // if
  else {
    gtemplambdaList = new Templambda();
    CopyTree( parameter, gtemplambdaList->parameter );
    CopyTree( statement, gtemplambdaList->statement );
    sprintf( gtemplambdaList->name, "%d", gfunctionnum );
    ++gfunctionnum;
    gtemplambdaList->next = NULL;
  } // else


} // NewTemplambda()

void CleanTemplambdaList( TemplambdaList &gTemplambdaList ) {
  TemplambdaList runner = gTemplambdaList;
  while ( runner != NULL ) {
    CleanTree( gTemplambdaList->parameter );
    CleanTree( gTemplambdaList->statement );
    runner = runner->next;
  } // while

  gTemplambdaList = NULL;
  delete gTemplambdaList; 
} // CleanTemplambdaList()

/*
void Checkdelist( AreavariableList gdeList, DefinitionList nownode ) {
  AreavariableList runarea = gdeList;
  DefinitionList runner = nownode;
  for ( ; runner != NULL ; runner = runner->next ) {
    cout << "name: " << runner->name << "\thead :";
    Preorder( runner->head, false, 0 ); 
  }  // for

  cout << "**************" << endl;
  if ( runarea->next != NULL )
    nownode = runarea->linker;
  runarea = runarea->next;
  while ( runarea != NULL ) {
    runner = nownode;
    while ( runner != NULL ) {
      cout << "name: " << runner->name << "\thead :";
      Preorder( runner->head, false, 0 ); 
      runner = runner->next;
    } // while

    cout << "***************" << endl;
    if ( runarea->next != NULL )
      nownode = runarea->linker;
    runarea = runarea->next;
  } // while
} // Checkdelist()
*/
bool NodeshaveQuote( BTree tree ) {
  if ( tree != NULL ) {
    if ( !tree->hasquote )
      return false;
    else if ( NodeshaveQuote( tree->left ) && NodeshaveQuote( tree->right ) )
      return true;
    return false;
  } // if
  
  return true;
} // NodeshaveQuote()

bool Preorder_Evalu( BTree linker, BTree &head, bool isfirstlevel, 
                     AreavariableList nowdeList, DefinitionList nownode, bool infunction,
                     bool thelastargument, bool incondition, bool intestcondition ) { 
  // linker是 symbol or instruction 之前的 node )
  SStr origincommand;
  if ( strcmp( head->token, "(" ) == 0 ) {    // 因通過syntax error 所以碰到括弧必定會有左子樹 因此不需要 head->left != NULL )
    if ( head->hasquote ) {
      gdontprint = false;
      return true;
    } // if

    if ( strcmp( head->type, "QUOTE" )  == 0 ) {
      // check error
      // if no error , then...
      SStr quote;
      strcpy( quote, "quote" ) ;
      if ( !Argumentnum( 1, 1, head, quote, true ) ) {
        gevaluerror = true;
        return false;
      } // if

      Quoter( head->left );
      if ( linker != NULL ) {
        head = head->left;
        linker->left = head;
      } // if
      else    
        head = head->left;
      gdontprint = false;
      return true;
    } // if

    BTree originhead = NULL;
    CopyTree( head, originhead );

    if ( head->left != NULL && strcmp( head->left->type, "LEFT-PAREN" ) == 0 ) { 
      // 主要用來處理有兩個括弧相連的情形 ex:"((car cons cons) '1 '2)" ) 
      if ( !Preorder_Evalu( head, head->left, false, nowdeList, nownode, infunction, true,
                            false, false ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        gnoreturnvalue = false; 
        return false;
      } // if
    } // if


    if ( strcmp( head->left->type, "SYMBOL" ) == 0 ) { 
      // 處理symbol代表的指令 ex:處理( abc a b)中的abc ) 
      SStr originname;
      strcpy( originname, "" );
      ChecknameinDeList( head->left->token, originname, nowdeList, nowdeList->list ) ;
      if ( strcmp( originname, "" ) == 0 ) { // not defined symbol )
        cout << "ERROR (unbound symbol) : " << head->left->token << endl; 
        gevaluerror = true;
        CleanTree( originhead );
        return false; 
      } // if
      else if ( IsInstruction( originname ) ) { // 是指令 ) 
        strcpy( head->left->token, originname );
      } // else if
      else {  // (->left 是 "defined symbol", 但不是指令 )
        if ( !Preorder_Evalu( head, head->left, false, nowdeList, nownode, infunction, true,
                              false, false ) ) {
          CleanTree( originhead );
          gnoreturnvalue = false;
          return false;
        } // if   
      } // else
    } // if
    
    /*
    if ( gturnon ) {
    Preorder( head, false, 0 );
    system("pause");
    } // if
    */

    if ( head->left != NULL && strcmp( head->left->type, "PREFUNCTION" ) == 0 
         && CheckifisFunctioninDeList( head->left->address, nowdeList, nownode ) ) {
      SStr originname ;
      strcpy( originname, head->left->address );
      AreavariableList maygotonext = nowdeList;
      DefinitionList maygotonextnode = nownode;
      // Checkdelist( maygotonext, maygotonextnode );
      // system("pause");
      if ( CheckifinDeList_S( originname, head->left, maygotonext, maygotonextnode ) ) {
        // Checkdelist( maygotonext, maygotonextnode );
        // system("pause");
        if ( maygotonext->next != NULL ) {
          maygotonext = maygotonext->next;
          maygotonextnode = maygotonext->linker;
        } // if
        else {
          maygotonext = maygotonext;
          maygotonextnode = maygotonextnode->next;
        } // else

        if ( ( strcmp( head->left->token, "(" ) == 0 
               || strcmp( head->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( head, head->left, false, maygotonext, maygotonextnode, infunction, true,
                                 false, false  ) )
          cout << "error in transfering procedure function to runner->head";
      } // if
    } // if
    else if ( head->left != NULL && strcmp( head->left->type, "INSTRUCTION" ) == 0 ) { 
      // 是instruction的procedure表示法 ex: cons的procedure表示為 #<procedure cons> ) 
      ProceduretoIns( head->left ); // 將procedure表示法轉換成指令 ) 
    } // else if

    if ( !head->left->hasquote && strcmp( head->left->token, "quote" ) == 0 ) {
      // check error
      // if no error , then...
      BTree runner = head->right;
      if ( ! Argumentnum( 1, 1, runner, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      Quoter( head->right->left );
      if ( linker != NULL )
        linker->left = head->right->left;
      else
        head = head->right->left;
      gdontprint = false;
      CleanTree( originhead );
      return true;   
    } // if
    else if ( !head->left->hasquote && strcmp( head->left->token, "cons" ) == 0 ) {
      // check error
      // if no error , then...
      BTree runner = head->right;
      if ( ! Argumentnum( 2, 2, runner, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      if ( !Islist2( head ) ) {
        cout << "ERROR (non-list) : ";
        Preorder( head, false, 0 );
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; i < 2 ; runner = runner->right, ++i ) {  // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 ) 
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      head = head->right;
      head->right = head->right->left;
      if ( linker != NULL )
        linker->left = head;
      gdontprint = false;
      CleanTree( originhead );
      return true;        
      // check error
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "define" ) == 0 ) {
      // check error
      // if no error , then...)
      bool firsttype = true; // ( define Symbol Tree )
      if ( !isfirstlevel ) {
        gevaluerror = true;
        cout << "ERROR (level of DEFINE)" << endl;
        CleanTree( originhead );
        return false;
      } // if

      BTree runner = head->right;
      if ( runner == NULL || ( runner != NULL && strcmp( runner->type, "NIL" ) == 0 )
           || runner->left == NULL || runner->right == NULL 
           || ( runner->right != NULL && strcmp( runner->right->type, "NIL" ) == 0 ) 
           || runner->right->left == NULL || IsInstruction( runner->left->token ) || 
           ! ( strcmp( runner->left->type, "SYMBOL" ) == 0 || 
               strcmp( runner->left->type, "LEFT-PAREN" ) == 0 ) ||
           ! Argumentnum( 2, 9999, head->right, head->left->token, false ) ) {
        cout << "ERROR (DEFINE format) : " ;
        Preorder( head, false, 0 );
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      if ( strcmp( runner->left->type, "LEFT-PAREN" ) == 0 ) { // check format error
        for ( BTree runner2 = runner->left ; runner2 != NULL && strcmp( runner2->left->type, "NIL" ) != 0 ; 
              runner2 = runner2->right ) {
          if ( strcmp( runner2->left->type, "SYMBOL" ) != 0 ) {
            cout << "ERROR (DEFINE format) : " ;
            Preorder( head, false, 0 );
            gevaluerror = true;
            CleanTree( originhead );
            return false;
          } // if
        } // for
        
        firsttype = false;
      } // if
      /*
      runner = head->right->right;
      for ( int i = 1 ; i < 2 ; runner = runner->right, ++i ) {  // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( strcmp( runner->left->token, "(" ) == 0 
             && !Preorder_Evalu( runner, runner->left, false ) ) {
          gevaluerror = true;
          return false;
        } // if
      } // for 
      */
      if ( !firsttype ) { // proj3
	      // transfer (define (x1 ...) .... )  to (define x1 (lambda.....) )
        BTree lambdahead = NULL;
        SStr command = "lambda";
        BeCommendhead( lambdahead, command );
        lambdahead->right = new STree(); // ( lambda "(...)" .... ) 
        strcpy( lambdahead->right->token, "(" );
        strcpy( lambdahead->right->type, "LEFT-PAREN" );
        if ( head->right->left->right != NULL && strcmp( head->right->left->right->type, "NIL" ) != 0 )
          CopyTree( head->right->left->right, lambdahead->right->left );
        else {
          lambdahead->right->left = new STree();
          strcpy( lambdahead->right->left->token, "nil" );
          strcpy( lambdahead->right->left->type, "NIL" );
          lambdahead->right->left->left = NULL;
          lambdahead->right->left->right = NULL;
        } // else
        
        lambdahead->right->right = NULL;

        BTree runner = head->right->right;
        BTree builder = lambdahead->right; 
        for ( ; runner != NULL && strcmp( runner->type, "NIL" ) != 0 ; runner = runner->right ) {
          // store statement )
          builder->right = new STree();
          strcpy( builder->right->token, "(" );
          strcpy( builder->right->type, "LEFT-PAREN" );
          CopyTree( runner->left, builder->right->left );
          if ( runner != NULL && strcmp( runner->type, "NIL" ) != 0 ) {
            builder = builder->right;
          } // if
          else
            builder->right->right = NULL;
        } // for

        AreavariableList  maygotonext = nowdeList;
        DefinitionList maygotonextnode = nownode;
        if ( maygotonext->next != NULL ) {
          maygotonextnode = maygotonext->linker;
          maygotonext = maygotonext->next;
        } // if

        bool notlocal = gnotlocalarea;
        if ( !Preorder_Evalu( NULL, lambdahead, false, maygotonext, maygotonextnode, infunction, true,
                              false, false ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      
        // if ( Checkifin1DeList( head->right->left->left->token ) )
        NewDefinition( nowdeList, head->right->left->left->token, lambdahead, true );
        CleanTree( lambdahead );
        // Rewritedelist( head->right->left->left->token, lambdahead, true );
        // else 
        //  NewDefinition( head->right->left->left->token, lambdahead, true );
        if ( notlocal ) 
          cout << head->right->left->left->token << " defined" << endl ;
      } // if
      else {
        // if ( Checkifin1DeList( head->right->left->token ) )
        AreavariableList  maygotonext = nowdeList;
        DefinitionList maygotonextnode = nownode;
        if ( maygotonext->next != NULL ) {
          maygotonextnode = maygotonext->linker;
          maygotonext = maygotonext->next;
        } // if

        bool notlocal = gnotlocalarea;
        runner = head->right->right;
        for ( int i = 1 ; i < 2 ; runner = runner->right, ++i ) {  // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
          if ( strcmp( runner->left->token, "(" ) == 0 
               && !Preorder_Evalu( runner, runner->left, false, 
                                   maygotonext, maygotonextnode, infunction, true,
                                   false, false  ) ) {
            gevaluerror = true;
            gnoreturnvalue = false;
            CleanTree( originhead );
            return false;
          } // if
        } // for 
    
        NewDefinition( nowdeList, head->right->left->token, head->right->right->left, false );
        // Rewritedelist( head->right->left->token, head->right->right->left, false );
        // else 
        // NewDefinition( head->right->left->token, head->right->right->left, false );
        if ( notlocal ) 
          cout << head->right->left->token << " defined" << endl ;
      } // else

      
      gdontprint = true;
      CleanTree( originhead );
      return true;
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "list" ) == 0 ) {
      // check error
      // if no error , then...
      BTree runner = head->right;
      if ( !Islist2( head ) ) {
        cout << "ERROR (non-list) : ";
        Preorder( head, false, 0 );
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      runner = head->right;
      if ( runner == NULL || ( runner != NULL && strcmp( runner->type, "NIL" ) == 0 ) ) {
        strcpy( head->type, "NIL" );
        strcpy( head->token, "nil" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      } // if

      for ( int i = 0 ; runner != NULL 
            && strcmp( runner->type, "NIL" ) != 0 ; runner = runner->right, ++i ) {  
        // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 ) 
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false  ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      if ( linker != NULL )
        linker->left = head->right;
      else 
        head = head->right;
      gdontprint = false;
      CleanTree( originhead );
      return true;   
    } // else if      
    else if ( !head->left->hasquote && strcmp( head->left->token, "car" ) == 0 ) {
      // check error
      // if no error , then...
      BTree runner = head->right;
      if ( ! Argumentnum( 1, 1, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; i < 1 ; runner = runner->right, ++i ) {  // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 ) 
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false  ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      runner = head->right;
      if ( strcmp( runner->left->token, "(" ) != 0 ) { // argument type需為 tree )
        cout << "ERROR (car with incorrect argument type) : " ;
        Preorder( runner->left, false, 0 );
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if
      
      gdontprint = false;
      if ( Isdottedpair( head->right->left ) ) {
        if ( linker != NULL )
          linker->left = head->right->left->left; // S-EXP外不含instruction之"()"
        else
          head = head->right->left->left;
        CleanTree( originhead );
        return true;   
      } // if
      else {
        head = head->right->left->left;
        CleanTree( originhead );
        return true;
      } // else 
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "cdr" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 1, 1, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; i < 1 ; runner = runner->right, ++i ) {  // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 ) 
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false  ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      runner = head->right;
      if ( !strcmp( runner->left->token, "(" ) == 0 ) { // argument type需為 tree )
        cout << "ERROR (cdr with incorrect argument type) : ";
        Preorder( runner->left, false, 0 ); 
        gevaluerror = true;
        CleanTree( originhead );
        return false; 
      } // if

      gdontprint = false;
      if ( Isdottedpair( head->right->left ) ) { 
        if ( linker != NULL )
          linker->left = head->right->left->right; // S-EXP外不含instruction之"()" 
        else  
          head = head->right->left->right;
        CleanTree( originhead );
        return true;
      } // if
      else { // 無右子樹 )
        strcpy( head->token, "nil" );
        strcpy( head->type, "NIL" );
        strcpy( head->address, head->right->left->address );
        CleanTree( head->left );
        CleanTree( head->right );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      } // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "atom?" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 1, 1, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; i < 1 ; runner = runner->right, ++i ) {  // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 ) 
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false  ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      if ( IsatomT( head->right ) ) {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "#t" );
        strcpy( head->type, "T" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      } // if
      else {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "nil" );
        strcpy( head->type, "NIL" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;    
      } // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "pair?" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 1, 1, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; i < 1 ; runner = runner->right, ++i ) {  // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 ) 
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false  ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      if ( Ispair( head->right ) ) {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "#t" );
        strcpy( head->type, "T" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      } // if
      else {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "nil" );
        strcpy( head->type, "NIL" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;    
      } // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "list?" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 1, 1, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; i < 1 ; runner = runner->right, ++i ) {  // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false  ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      if ( Islist2( head->right->left ) ) {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "#t" );
        strcpy( head->type, "T" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      }  // if
      else {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "nil" );
        strcpy( head->type, "NIL" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;         
      }  // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "null?" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 1, 1, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; i < 1 ; runner = runner->right, ++i ) {  // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false  ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      if ( Isnull( head->right ) ) {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "#t" );
        strcpy( head->type, "T" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      }  // if
      else {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "nil" );
        strcpy( head->type, "NIL" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;         
      }  // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "integer?" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 1, 1, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; i < 1 ; runner = runner->right, ++i ) {  // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false  ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      if ( IsInteger( head->right ) ) {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "#t" );
        strcpy( head->type, "T" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      }  // if
      else {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "nil" );
        strcpy( head->type, "NIL" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;         
      }  // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "real?" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 1, 1, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; i < 1 ; runner = runner->right, ++i ) {  // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false  ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      if ( Isnumber( head->right ) ) {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "#t" );
        strcpy( head->type, "T" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      }  // if
      else {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "nil" );
        strcpy( head->type, "NIL" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;         
      }  // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "number?" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 1, 1, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; i < 1 ; runner = runner->right, ++i ) {  // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false  ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      if ( Isnumber( head->right ) ) {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "#t" );
        strcpy( head->type, "T" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      }  // if
      else {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "nil" );
        strcpy( head->type, "NIL" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;         
      }  // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "string?" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 1, 1, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; i < 1 ; runner = runner->right, ++i ) {  // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false  ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      if ( Isstring( head->right ) ) {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "#t" );
        strcpy( head->type, "T" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      }  // if
      else {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "nil" );
        strcpy( head->type, "NIL" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;         
      }  // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "boolean?" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 1, 1, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; i < 1 ; runner = runner->right, ++i ) {  // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      if ( Isboolean( head->right ) ) {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "#t" );
        strcpy( head->type, "T" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      }  // if
      else {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "nil" );
        strcpy( head->type, "NIL" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;         
      }  // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "symbol?" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 1, 1, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; i < 1 ; runner = runner->right, ++i ) {  // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false  ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      if ( Issymbol( head->right ) ) {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "#t" );
        strcpy( head->type, "T" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      }  // if
      else {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "nil" );
        strcpy( head->type, "NIL" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;         
      }  // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "+" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 2, 9999, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; runner != NULL 
            && strcmp( runner->type, "NIL" ) != 0 ; runner = runner->right, ++i ) {  
        // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      int ans1 = atoi( head->right->left->token );
      float ans2 = atof( head->right->left->token );
      bool hasfloat = false;
      if ( strcmp( head->right->left->type, "INT" ) != 0 
           && strcmp( head->right->left->type, "FLOAT" ) != 0 ) {
        cout << "ERROR (+ with incorrect argument type) : " ;
        Preorder( head->right->left, false, 0 );
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      if ( strcmp( head->right->left->type, "FLOAT" ) == 0 )
        hasfloat = true;       
      runner = head->right->right;
      while ( runner != NULL && strcmp( runner->type, "NIL" ) != 0 ) {
        if ( strcmp( runner->left->type, "INT" ) != 0 && strcmp( runner->left->type, "FLOAT" ) != 0 ) {
          cout << "ERROR (+ with incorrect argument type) : " ;
          Preorder( runner->left, false, 0 );
          gevaluerror = true;
          CleanTree( originhead );
          return false;
        } // if

        if ( strcmp( runner->left->type, "FLOAT" ) == 0 ) 
          hasfloat = true;
        ans1 = ans1+atoi( runner->left->token ) ;
        ans2 = ans2+atof( runner->left->token ) ;
        runner = runner->right;
      } // while
      
      CleanTree( head );
      head = new STree();
      if ( hasfloat ) {
        Fota( head->token, ans2 ) ;
        strcpy( head->type, "FLOAT" ) ;
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      } // if
      else {
        sprintf( head->token, "%d", ans1 ); // 10表示為將ans1轉換成10進制 但在此處無作用 本來就是10進制) 
        strcpy( head->type, "INT" ) ;
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      } // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "-" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 2, 9999, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; runner != NULL 
            && strcmp( runner->type, "NIL" ) != 0 ; runner = runner->right, ++i ) {  
        // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      int ans1 = atoi( head->right->left->token );
      float ans2 = atof( head->right->left->token );
      bool hasfloat = false;
      if ( strcmp( head->right->left->type, "INT" ) != 0 
           && strcmp( head->right->left->type, "FLOAT" ) != 0 ) {
        cout << "ERROR (- with incorrect argument type) : " ;
        Preorder( head->right->left, false, 0 );
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      if ( strcmp( head->right->left->type, "FLOAT" ) == 0 )
        hasfloat = true;        
      runner = head->right->right;
      while ( runner != NULL && strcmp( runner->type, "NIL" ) != 0 ) {
        if ( strcmp( runner->left->type, "INT" ) != 0 && strcmp( runner->left->type, "FLOAT" ) != 0 ) {
          cout << "ERROR (- with incorrect argument type) : " ;
          Preorder( runner->left, false, 0 );
          gevaluerror = true;
          CleanTree( originhead );
          return false;
        } // if

        if ( strcmp( runner->left->type, "FLOAT" ) == 0 )
          hasfloat = true;
        ans1 = ans1-atoi( runner->left->token ) ;
        ans2 = ans2-atof( runner->left->token ) ;
        runner = runner->right;
      } // while
      
      CleanTree( head );
      head = new STree();
      if ( hasfloat ) {
        Fota( head->token, ans2 ) ;
        strcpy( head->type, "FLOAT" ) ;
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      } // if
      else {
        sprintf( head->token, "%d", ans1 ); // 10表示為將ans1轉換成10進制 但在此處無作用 本來就是10進制) 
        strcpy( head->type, "INT" ) ;
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      } // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "*" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 2, 9999, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; runner != NULL 
            && strcmp( runner->type, "NIL" ) != 0 ; runner = runner->right, ++i ) {  
        // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      int ans1 = atoi( head->right->left->token );
      float ans2 = atof( head->right->left->token );
      bool hasfloat = false;
      if ( strcmp( head->right->left->type, "INT" ) != 0 
           && strcmp( head->right->left->type, "FLOAT" ) != 0 ) {
        cout << "ERROR (* with incorrect argument type) : " ;
        Preorder( head->right->left, false, 0 );
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      if ( strcmp( head->right->left->type, "FLOAT" ) == 0 )
        hasfloat = true;         
      runner = head->right->right;
      while ( runner != NULL && strcmp( runner->type, "NIL" ) != 0 ) {
        if ( strcmp( runner->left->type, "INT" ) != 0 && strcmp( runner->left->type, "FLOAT" ) != 0 ) {
          cout << "ERROR (* with incorrect argument type) : " ;
          Preorder( runner->left, false, 0 );
          gevaluerror = true;
          CleanTree( originhead );
          return false;
        } // if

        if ( strcmp( runner->left->type, "FLOAT" ) == 0 ) 
          hasfloat = true;
        ans1 = ans1*atoi( runner->left->token ) ;
        ans2 = ans2*atof( runner->left->token ) ;
        runner = runner->right;
      } // while
      
      CleanTree( head );
      head = new STree();
      if ( hasfloat ) {
        Fota( head->token, ans2 ) ;
        strcpy( head->type, "FLOAT" ) ;
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      } // if
      else {
        sprintf( head->token, "%d", ans1 ); // 10表示為將ans1轉換成10進制 但在此處無作用 本來就是10進制) 
        strcpy( head->type, "INT" ) ;
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      } // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "/" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 2, 9999, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; runner != NULL 
            && strcmp( runner->type, "NIL" ) != 0 ; runner = runner->right, ++i ) {  
        // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      int ans1 = atoi( head->right->left->token );
      float ans2 = atof( head->right->left->token );
      bool hasfloat = false;
      if ( strcmp( head->right->left->type, "INT" ) != 0 
           && strcmp( head->right->left->type, "FLOAT" ) != 0 ) {
        cout << "ERROR (/ with incorrect argument type) : " ;
        Preorder( head->right->left, false, 0 );
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      if ( strcmp( head->right->left->type, "FLOAT" ) == 0 )
        hasfloat = true;          
      runner = head->right->right;
      while ( runner != NULL && strcmp( runner->type, "NIL" ) != 0 ) {
        if ( strcmp( runner->left->type, "INT" ) != 0 && strcmp( runner->left->type, "FLOAT" ) != 0 ) {
          cout << "ERROR (/ with incorrect argument type) : " ;
          Preorder( runner->left, false, 0 );
          gevaluerror = true;
          CleanTree( originhead );
          return false;
        } // if

        if ( strcmp( runner->left->token, "0" ) == 0 ) {
          cout << "ERROR (division by zero) : /" << endl;
          gevaluerror = true;
          CleanTree( originhead );
          return false;
        } // if

        if ( strcmp( runner->left->type, "FLOAT" ) == 0 ) 
          hasfloat = true;
        ans1 = ans1/atoi( runner->left->token ) ;
        ans2 = ans2/atof( runner->left->token ) ;
        runner = runner->right;
      } // while
      
      CleanTree( head );
      head = new STree();
      if ( hasfloat ) {
        Fota( head->token, ans2 ) ;
        strcpy( head->type, "FLOAT" ) ;
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      } // if
      else {
        sprintf( head->token, "%d", ans1 ); // 10表示為將ans1轉換成10進制 但在此處無作用 本來就是10進制) 
        strcpy( head->type, "INT" ) ;
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      } // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "not" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 1, 1, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; i < 1 ; runner = runner->right, ++i ) { 
        // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false  ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      if ( Isnull( head->right ) ) {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "#t" );
        strcpy( head->type, "T" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      }  // if
      else {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "nil" );
        strcpy( head->type, "NIL" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;         
      }  // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "and" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 2, 9999, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      
      for ( int i = 0 ; runner->right != NULL 
            && strcmp( runner->right->type, "NIL" ) != 0 ; runner = runner->right, ++i ) {  
        // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 true, false ) ) {
          if ( gnoreturnvalue ) {
            gevaluerror = true;
            gnoreturnvalue = false;
            CleanTree( originhead );
            return false;
          } // if
        } // if

        if ( runner->left != NULL && strcmp( runner->left->type, "NIL" ) == 0 )  {
          head = runner->left;
          CleanTree( originhead );
          return true;
        } // if

      } // for

      if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
           && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                               true, false ) ) {
        gevaluerror = true;
        gnoreturnvalue = false;
        CleanTree( originhead );
        return false;
      } // if

      gdontprint = false;
      head = runner->left;
      CleanTree( originhead );
      return true;
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "or" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 2, 9999, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; runner->right != NULL 
            && strcmp( runner->right->type, "NIL" ) != 0 ; runner = runner->right, ++i ) {  
        // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 true, false ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if

        if ( runner->left != NULL && strcmp( runner->left->type, "NIL" ) != 0 ) { 
          head = runner->left;
          CleanTree( originhead );
          return true;
        } // if

      } // for

      gdontprint = false;
      if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
           && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                               true, false ) ) {
        gevaluerror = true;
        gnoreturnvalue = false;
        CleanTree( originhead );
        return false;
      } // if

      head = runner->left;
      CleanTree( originhead );
      return true;
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, ">" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 2, 9999, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; runner != NULL 
            && strcmp( runner->type, "NIL" ) != 0 ; runner = runner->right, ++i ) {  
        // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      if ( ! ( strcmp( head->right->left->type, "FLOAT" ) == 0 
               || strcmp( head->right->left->type, "INT" ) == 0 ) ) { 
        cout << "ERROR (> with incorrect argument type) : ";
        Preorder( runner->left, false, 0 );
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      float ans2 = atof( head->right->left->token );
      bool fact = true;        
      runner = head->right->right;
      while ( runner != NULL && strcmp( runner->type, "NIL" ) != 0 ) {
        if ( ! ( strcmp( runner->left->type, "FLOAT" ) == 0 
                 || strcmp( runner->left->type, "INT" ) == 0 ) ) { 
          cout << "ERROR (> with incorrect argument type) : ";
          Preorder( runner->left, false, 0 );
          gevaluerror = true;
          CleanTree( originhead );
          return false;
        } // if

        if ( ! ( ans2 > atof( runner->left->token ) ) )
          fact = false;
        else
          ans2 = atof( runner->left->token );
        runner = runner->right;
      } // while
      
      if ( fact ) {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "#t" );
        strcpy( head->type, "T" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      }  // if
      else {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "nil" );
        strcpy( head->type, "NIL" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;         
      }  // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "<" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 2, 9999, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; runner != NULL 
            && strcmp( runner->type, "NIL" ) != 0 ; runner = runner->right, ++i ) {  
        // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      if ( ! ( strcmp( head->right->left->type, "FLOAT" ) == 0 
               || strcmp( head->right->left->type, "INT" ) == 0 ) ) { 
        cout << "ERROR (< with incorrect argument type) : ";
        Preorder( runner->left, false, 0 );
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      float ans2 = atof( head->right->left->token );
      bool fact = true;        
      runner = head->right->right;
      while ( runner != NULL && strcmp( runner->type, "NIL" ) != 0 ) {
        if ( ! ( strcmp( runner->left->type, "FLOAT" ) == 0 
                 || strcmp( runner->left->type, "INT" ) == 0 ) ) { 
          cout << "ERROR (< with incorrect argument type) : ";
          Preorder( runner->left, false, 0 );
          gevaluerror = true;
          CleanTree( originhead );
          return false;
        } // if

        if ( ! ( ans2 < atof( runner->left->token ) ) )
          fact = false;
        else
          ans2 = atof( runner->left->token );
        runner = runner->right;
      } // while
      
      if ( fact ) {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "#t" );
        strcpy( head->type, "T" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      }  // if
      else {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "nil" );
        strcpy( head->type, "NIL" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;         
      }  // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, ">=" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 2, 9999, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; runner != NULL 
            && strcmp( runner->type, "NIL" ) != 0 ; runner = runner->right, ++i ) {  
        // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      if ( ! ( strcmp( head->right->left->type, "FLOAT" ) == 0 
               || strcmp( head->right->left->type, "INT" ) == 0 ) ) { 
        cout << "ERROR (>= with incorrect argument type) : ";
        Preorder( runner->left, false, 0 );
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      float ans2 = atof( head->right->left->token );
      bool fact = true;        
      runner = head->right->right;
      while ( runner != NULL && strcmp( runner->type, "NIL" ) != 0  ) {
        if ( ! ( strcmp( runner->left->type, "FLOAT" ) == 0 
                 || strcmp( runner->left->type, "INT" ) == 0 ) ) { 
          cout << "ERROR (>= with incorrect argument type) : ";
          Preorder( runner->left, false, 0 );
          gevaluerror = true;
          CleanTree( originhead );
          return false;
        } // if

        if ( ! ( ans2 >= atof( runner->left->token ) ) )
          fact = false;
        else
          ans2 = atof( runner->left->token );
        runner = runner->right;
      } // while
      
      if ( fact ) {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "#t" );
        strcpy( head->type, "T" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      }  // if
      else {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "nil" );
        strcpy( head->type, "NIL" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;         
      }  // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "<=" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 2, 9999, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; runner != NULL 
            && strcmp( runner->type, "NIL" ) != 0 ; runner = runner->right, ++i ) {  
        // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      if ( ! ( strcmp( head->right->left->type, "FLOAT" ) == 0 
               || strcmp( head->right->left->type, "INT" ) == 0 ) ) { 
        cout << "ERROR (<= with incorrect argument type) : ";
        Preorder( runner->left, false, 0 );
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      float ans2 = atof( head->right->left->token );
      bool fact = true;        
      runner = head->right->right;
      while ( runner != NULL && strcmp( runner->type, "NIL" ) != 0 ) {
        if ( ! ( strcmp( runner->left->type, "FLOAT" ) == 0 
                 || strcmp( runner->left->type, "INT" ) == 0 ) ) { 
          cout << "ERROR (<= with incorrect argument type) : ";
          Preorder( runner->left, false, 0 );
          gevaluerror = true;
          CleanTree( originhead );
          return false;
        } // if

        if ( ! ( ans2 <= atof( runner->left->token ) ) )
          fact = false;
        else
          ans2 = atof( runner->left->token );
        runner = runner->right;
      } // while
      
      if ( fact ) {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "#t" );
        strcpy( head->type, "T" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      }  // if
      else {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "nil" );
        strcpy( head->type, "NIL" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;         
      }  // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "=" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 2, 9999, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; runner != NULL 
            && strcmp( runner->type, "NIL" ) != 0 ; runner = runner->right, ++i ) {  
        // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      if ( ! ( strcmp( head->right->left->type, "FLOAT" ) == 0 
               || strcmp( head->right->left->type, "INT" ) == 0 ) ) { 
        cout << "ERROR (= with incorrect argument type) : ";
        Preorder( runner->left, false, 0 );
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      float ans2 = atof( head->right->left->token );
      bool fact = true;        
      runner = head->right->right;
      while ( runner != NULL && strcmp( runner->type, "NIL" ) != 0 ) {
        if ( ! ( strcmp( runner->left->type, "FLOAT" ) == 0 
                 || strcmp( runner->left->type, "INT" ) == 0 ) ) { 
          cout << "ERROR (= with incorrect argument type) : ";
          Preorder( runner->left, false, 0 );
          gevaluerror = true;
          CleanTree( originhead );
          return false;
        } // if

        if ( ! ( ans2 == atof( runner->left->token ) ) )
          fact = false;
        else
          ans2 = atof( runner->left->token );
        runner = runner->right;
      } // while
      
      if ( fact ) {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "#t" );
        strcpy( head->type, "T" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      }  // if
      else {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "nil" );
        strcpy( head->type, "NIL" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;         
      }  // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "string-append" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 2, 9999, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; runner != NULL 
            && strcmp( runner->type, "NIL" ) != 0 ; runner = runner->right, ++i ) {  
        // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      SStr str;
      if ( !Islist2( head ) ) {
        cout << "ERROR (non-list) : ";
        Preorder( head, false, 0 );
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      if ( strcmp( head->right->left->type, "STRING" ) != 0 ) {
        cout << "ERROR (string-append with incorrect argument type) : ";
        Preorder( head->right->left, false, 0 );
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      strcpy( str, head->right->left->token );
      runner = head->right->right;
      int loc = 0;
      while ( runner != NULL && strcmp( runner->type, "NIL" ) != 0 ) {
        if ( strcmp( runner->left->type, "STRING" ) != 0 ) { 
          cout << "ERROR (string-append with incorrect argument type) : ";
          Preorder( runner->left, false, 0 );
          gevaluerror = true;
          CleanTree( originhead );
          return false;
        } // if

        loc = strlen( str )-1;
        for ( int i = 1 ; i < strlen( runner->left->token ) ; ++i, ++loc ) {
          str[loc] = runner->left->token[i];
        } // for

        str[loc] = '\0';
        runner = runner->right;
      } // while
   
      CleanTree( head );
      head = new STree();
      strcpy( head->token, str ) ;
      strcpy( head->type, "STRING" ) ;
      head->left = NULL;
      head->right = NULL;
      CleanTree( originhead );
      return true;
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "string>?" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 2, 9999, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; runner != NULL ; runner = runner->right, ++i ) {  
        // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      SStr compare;
      if ( !Islist2( head ) ) {
        cout << "ERROR (non-list) : ";
        Preorder( head, false, 0 );
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      if ( strcmp( head->right->left->type, "STRING" ) != 0 ) {
        cout << "ERROR (string>? with incorrect argument type) : ";
        Preorder( head->right->left, false, 0 );
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      strcpy( compare, head->right->left->token );
      bool fact = true;
      bool hasans = false;     
      runner = head->right->right;
      while ( runner != NULL && strcmp( runner->type, "NIL" ) != 0  ) {
        if ( strcmp( runner->left->type, "STRING" ) != 0 ) { 
          cout << "ERROR (string>? with incorrect argument type) : ";
          Preorder( runner->left, false, 0 );
          gevaluerror = true;
          CleanTree( originhead );
          return false;
        } // if

        for ( int i  = 0 ; i < strlen( compare ) && i < strlen( runner->left->token ) && !hasans ; ++i ) {
          if (  compare[i] < runner->left->token[i] ) {
            fact = false;
            hasans = true;
          } // if
          else if ( compare[i] > runner->left->token[i] )
            hasans = true;

        } // for
        
        if ( !hasans ) {  // 字串相同直到某一或兩者字串長度到尾 ) 
          if ( strlen( compare ) <= strlen( runner->left->token ) )
            fact = false;
        } // if

        strcpy( compare, runner->left->token ); 
        hasans = false;
        runner = runner->right;
      } // while
      
      if ( fact ) {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "#t" );
        strcpy( head->type, "T" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      }  // if
      else {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "nil" );
        strcpy( head->type, "NIL" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;         
      }  // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "string<?" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 2, 9999, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; runner != NULL 
            && strcmp( runner->type, "NIL" ) != 0 ; runner = runner->right, ++i ) {  
        // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      SStr compare;
      if ( !Islist2( head ) ) {
        cout << "ERROR (non-list) : ";
        Preorder( head, false, 0 );
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      if ( strcmp( head->right->left->type, "STRING" ) != 0 ) {
        cout << "ERROR (string<? with incorrect argument type) : ";
        Preorder( head->right->left, false, 0 );
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      strcpy( compare, head->right->left->token );
      bool fact = true;
      bool hasans = false;     
      runner = head->right->right;
      while ( runner != NULL && strcmp( runner->type, "NIL" ) != 0 ) {
        if ( strcmp( runner->left->type, "STRING" ) != 0 ) { 
          cout << "ERROR (string<? with incorrect argument type) : ";
          Preorder( runner->left, false, 0 );
          gevaluerror = true;
          CleanTree( originhead );
          return false;
        } // if

        for ( int i  = 0 ; i < strlen( compare ) && i < strlen( runner->left->token ) && !hasans ; ++i ) {
          if (  compare[i] > runner->left->token[i] ) {
            fact = false;
            hasans = true;
          } // if
          else if ( compare[i] < runner->left->token[i] )
            hasans = true;
        } // for
        
        if ( !hasans ) {  // 字串相同直到某一或兩者字串長度到尾 ) 
          if ( strlen( compare ) >= strlen( runner->left->token ) )
            fact = false;
        } // if

        strcpy( compare, runner->left->token ); 
        hasans = false;
        runner = runner->right;
      } // while
      
      if ( fact ) {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "#t" );
        strcpy( head->type, "T" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      }  // if
      else {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "nil" );
        strcpy( head->type, "NIL" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;         
      }  // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "string=?" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 2, 9999, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; runner != NULL 
            && strcmp( runner->type, "NIL" ) != 0 ; runner = runner->right, ++i ) {  
        // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      SStr compare;
      if ( !Islist2( head ) ) {
        cout << "ERROR (non-list) : ";
        Preorder( head, false, 0 );
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      if ( strcmp( head->right->left->type, "STRING" ) != 0 ) {
        cout << "ERROR (string>? with incorrect argument type) : ";
        Preorder( head->right->left, false, 0 );
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      strcpy( compare, head->right->left->token );
      bool fact = true;
      bool hasans = false;     
      runner = head->right->right;
      while ( runner != NULL && strcmp( runner->type, "NIL" ) != 0 ) {
        if ( strcmp( runner->left->type, "STRING" ) != 0 ) { 
          cout << "ERROR (string=? with incorrect argument type) : ";
          Preorder( runner->left, false, 0 );
          gevaluerror = true;
          CleanTree( originhead );
          return false;
        } // if

        for ( int i  = 0 ; i < strlen( compare ) && i < strlen( runner->left->token ) && !hasans ; ++i ) {
          if (  compare[i] < runner->left->token[i] ) {
            fact = false;
            hasans = true;
          } // if
          else if ( compare[i] > runner->left->token[i] ) {
            fact = false;
            hasans = true;
          } // else if
        } // for
        
        if ( !hasans ) {  // 字串相同直到某一或兩者字串長度到尾 ) 
          if ( strlen( compare ) != strlen( runner->left->token ) )
            fact = false;
        } // if


        strcpy( compare, runner->left->token ); 
        runner = runner->right;
      } // while
      
      if ( fact ) {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "#t" );
        strcpy( head->type, "T" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      }  // if
      else {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "nil" );
        strcpy( head->type, "NIL" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;         
      }  // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "eqv?" ) == 0 ) {
      BTree runner = head->right;
      if ( ! Argumentnum( 2, 2, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; i < 2 ; runner = runner->right, ++i ) {  // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      if ( Iseqv( head->right->left, head->right->right->left ) ) {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "#t" );
        strcpy( head->type, "T" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      }  // if
      else {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "nil" );
        strcpy( head->type, "NIL" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;         
      }  // else
      
            
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "equal?" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 2, 2, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      for ( int i = 0 ; i < 2 ; runner = runner->right, ++i ) {  // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      gdontprint = false;
      if ( Isequal( head->right->left, head->right->right->left ) ) {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "#t" );
        strcpy( head->type, "T" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;
      }  // if
      else {
        CleanTree( head );
        head = new STree();
        strcpy( head->token, "nil" );
        strcpy( head->type, "NIL" );
        head->left = NULL;
        head->right = NULL;
        CleanTree( originhead );
        return true;         
      }  // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "begin" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 1, 9999, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      bool thelastargu = true;
      for ( int i = 0 ; runner != NULL    // i < 3 ?
            && strcmp( runner->type, "NIL" ) != 0 ; runner = runner->right, ++i ) {  

        if ( runner != NULL && strcmp( runner->type, "NIL" ) != 0 )
          thelastargu = false;
        // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, thelastargu,
                                 false, false ) ) {
          if ( ! ( !thelastargu && gnoreturnvalue ) ) {
            gevaluerror = true;
            gnoreturnvalue = false;
            CleanTree( originhead );
            return false;
          } // if
          else {
            thelastargu = true;
            gnoreturnvalue = false;
          } // else
        } // if
      } // for

      gdontprint = false;
      runner = head->right;
      while ( runner->right != NULL && strcmp( runner->right->type, "NIL" ) != 0 )
        runner = runner->right;
      head = runner->left;
      CleanTree( originhead );
      return true;
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "if" ) == 0 ) {
      // check error
      BTree runner = head->right;
      if ( ! Argumentnum( 2, 3, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      BTree temp;
      CopyTree( head, temp ); 
      for ( int i = 0 ; i < 1 && runner != NULL 
            && strcmp( runner->type, "NIL" ) != 0 ; runner = runner->right, ++i ) {  
        // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
             && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                 false, false ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if
      } // for

      if ( strcmp( head->right->left->type, "NIL" ) != 0 ) {
        runner = head->right->right;
        if ( ( runner != NULL && strcmp( runner->type, "NIL" ) != 0 && runner->left != NULL
               && ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
               && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                   false, false ) ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if

        if ( !gevaluerror ) {
          head = head->right->right->left;
          gdontprint = false;
          CleanTree( originhead );
          return true;
        } // if
        else {
          CleanTree( originhead );
          return false;
        } // else
      } // if
      else if ( head->right->right != NULL && strcmp( head->right->right->type, "NIL" ) != 0
                && head->right->right->right != NULL && strcmp( head->right->right->right->type, "NIL" ) != 0
                && head->right->right->right->left != NULL ) { 
        //  if 中含有第3個sequence ) 
        runner = head->right->right->right;
        if ( ( runner != NULL && strcmp( runner->type, "NIL" ) != 0 && runner->left != NULL
               && ( strcmp( runner->left->token, "(" ) == 0 || strcmp( runner->left->type, "SYMBOL" ) == 0 )
               && !Preorder_Evalu( runner, runner->left, false, nowdeList, nownode, infunction, true,
                                   false, false ) ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if

        if ( !gevaluerror ) {
          head = head->right->right->right->left;
          gdontprint = false;
          CleanTree( originhead );
          return true;
        } // if
        else {
          CleanTree( originhead );
          return false;
        } // else
      } // else if
      else { // 條件為nil 且無第三個的seq )
        // ERROR()
        if ( incondition ) {
          cout << "ERROR (unbound condition) : ";
          Preorder( temp, false, 0 );
        } // if
        else if ( infunction )
          gnoreturnvalue = true;
        else {
          if ( thelastargument ) {
            cout << "ERROR (no return value) : ";
            Preorder( temp, false, 0 );
          } // if
          else
            gnoreturnvalue = true; 
        } // else

        CleanTree( originhead );
        return false;
      } // else
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "cond" ) == 0 ) {
      // check error
      /*
      BTree runner = head->right;
      for ( ; runner != NULL ; runner = runner->right ) {  // 走訪左子樹並檢查有無 symbol or instruction需要計算 )  
        BTree runner2 = runner->left;
        
        if ( runner2->left != NULL 
             && strcmp( runner2->left->token, "else" ) == 0 && runner->right == NULL ) {
        }  // if 假如最後一個seq為else, 不須計算 以避免 defined else )
        else {
          if (  int i = 0 ; i < 1 && runner2 != NULL ; ++i ) {
            if ( ( strcmp( runner2->left->token, "(" ) == 0 || strcmp( runner2->left->type, "SYMBOL" ) == 0 )
                   && !Preorder_Evalu( runner2, runner2->left, false ) ) {
          gevaluerror = true;
          return false;
        } // if
          } // if
        } // else
      } // for 
      */
      ++gcondnum;
      if ( ! Argumentnum( 1, 9999, head->right, head->left->token, false ) ) {
        cout << "ERROR (COND format) : " ;
        Preorder( head, false, 0 ) ;
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      BTree temp;
      BTree ans;  // 存放該取的argument ) 
      bool hasans = false;
      CopyTree( head, temp );
      BTree runner = head->right;
      for ( BTree runner2 = runner->left ; runner != NULL 
            && strcmp( runner->type, "NIL" ) != 0 ; runner = runner->right ) {
        runner2 = runner->left;
        if ( strcmp( runner2->type, "LEFT-PAREN" ) != 0 ) {
          cout << "ERROR (COND format) : ";
          Preorder( temp, false, 0 );
          CleanTree( originhead );
          return false;
        } // if
   
      }  // for

      runner = head->right;
      for ( BTree runner2 = runner->left ; runner != NULL  // caculate & check error ) 
            && strcmp( runner->type, "NIL" ) != 0 ; runner = runner->right ) {
        runner2 = runner->left;
        if ( !hasans && runner2->left != NULL 
             && ( runner->right == NULL || 
                  ( runner->right != NULL && strcmp( runner->right->type, "NIL" ) == 0 ) )
             && strcmp( runner2->left->token, "else" ) == 0 )  { // 碰到最終條件else的情形時 )
          runner2 = runner2->right;
          if ( runner2 == NULL || ( runner2 != NULL && strcmp( runner2->type, "NIL" ) == 0 ) ) {
            cout << "ERROR (COND format) : ";
            Preorder( temp, false, 0 );
            CleanTree( originhead );
            return false;
          } // if

          while ( !hasans && runner2->right != NULL 
                  && strcmp( runner2->right->type, "NIL" ) != 0 )  // 回傳last argument) 
            runner2 = runner2->right;
          if ( !hasans && runner2->left != NULL
               && ( strcmp( runner2->left->token, "(" ) == 0
                    || strcmp( runner2->left->type, "SYMBOL" ) == 0 )
               && !Preorder_Evalu( runner2, runner2->left, false, nowdeList, nownode, infunction, true,
                                   false, false ) ) {
            gevaluerror = true;
            gnoreturnvalue = false;
            CleanTree( originhead );
            return false;
          } // if

          if ( !hasans ) {
            CopyTree( runner2->left, ans );
            hasans = true;
          } // if
        } // if
        else {
          for (  int i = 0 ; !hasans && i < 1 && runner2 != NULL 
                && strcmp( runner2->type, "NIL" ) != 0 ; ++i ) { // 只是因為方便看程式碼所以用for )
            if ( ( strcmp( runner2->left->token, "(" ) == 0 || strcmp( runner2->left->type, "SYMBOL" ) == 0 )
                 && !Preorder_Evalu( runner2, runner2->left, false, nowdeList, nownode, infunction, true,
                                     false, false ) ) {
              gevaluerror = true;
              gnoreturnvalue = false;
              CleanTree( originhead );
              return false;
            } // if
          } // for

          if ( !hasans && 
               ( runner2 == NULL || ( runner2 != NULL && strcmp( runner2->type, "NIL" ) == 0 ) ) ) {
            cout << "ERROR (COND format) : ";
            Preorder( temp, false, 0 );
            CleanTree( originhead );
            return false;
          } // if

          if ( !hasans && strcmp( runner2->left->type, "NIL" ) != 0 )  { // 條件結果不為nil時 )
            runner2 = runner2->right;
            if ( runner2 == NULL || ( runner2 != NULL && strcmp( runner2->type, "NIL" ) == 0 ) ) {
              cout << "ERROR (COND format) : ";
              Preorder( temp, false, 0 );
              CleanTree( originhead );
              return false;
            } // if

            bool thelastargu = true;

            while ( !hasans && runner2->right != NULL && strcmp( runner2->right->type, "NIL" ) != 0 ) {
              if ( runner2->right != NULL && strcmp( runner2->right->type, "NIL" ) != 0 )
                thelastargu = false;
              if ( runner2->left != NULL 
                   && ( strcmp( runner2->left->token, "(" ) == 0 
                        || strcmp( runner2->left->type, "SYMBOL" ) == 0 )
                   && !Preorder_Evalu( runner2, runner2->left, false, nowdeList, nownode, 
                                       infunction, thelastargu, false, false ) ) {
                if ( ! ( !thelastargu && gnoreturnvalue ) ) {
                  gevaluerror = true;
                  gnoreturnvalue = false;
                  CleanTree( originhead );
                  return false;
                } // if
                else {
                  gnoreturnvalue = true;
                  thelastargu = true;
                } // else
              } // if

              runner2 = runner2->right;
            } // while

            if ( !hasans && runner2->left != NULL 
                 && ( strcmp( runner2->left->token, "(" ) == 0 
                      || strcmp( runner2->left->type, "SYMBOL" ) == 0 )
                 && !Preorder_Evalu( runner2, runner2->left, false, nowdeList, nownode, infunction, true,
                                     false, false ) ) {
              gevaluerror = true;
              gnoreturnvalue = false;
              CleanTree( originhead );
              return false;
            } // if

            if ( !hasans ) {
              CopyTree( runner2->left, ans );
              hasans = true;
            } // if
          } // if
          else {    // 條件nil的情況下  ) 
            runner2 = runner2->right;
            if ( runner2 == NULL || ( runner2 != NULL && strcmp( runner2->type, "NIL" ) == 0 ) ) {
              cout << "ERROR (COND format) : ";
              Preorder( temp, false, 0 );
              CleanTree( originhead );
              return false;
            } // if
          } // else
        } // else
      } // for

      // error : no return value )
      if ( hasans ) {
        head = ans;
        if ( linker != NULL )
          linker->left = head;
        gdontprint = false;
        CleanTree( originhead );
        return true;
      } // if

      if ( incondition ) {
        cout << "ERROR (unbound condition) : ";
        Preorder( temp, false, 0 );
      } // if
      else if ( infunction ) {
        gnoreturnvalue = true;
      } // if
      else {
        if ( thelastargument ) {
          cout << "ERROR (no return value) : ";
          Preorder( temp, false, 0 );
        } // if
        else
          gnoreturnvalue = true; 
      } // else

      CleanTree( originhead );
      return false; 
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "clean-environment" ) == 0 ) {
      // check error
      if ( !isfirstlevel ) {
        gevaluerror = true;
        cout << "ERROR (level of CLEAN-ENVIRONMENT)" << endl;
        CleanTree( originhead );
        return false;
      } // if

      if ( ! Argumentnum( 0, 0, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      gdontprint = true;
      Clean_environment( gdeList );
      cout << "environment cleaned" << endl;
      CleanTree( originhead );
      return true;
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "exit" ) == 0 ) {

      if ( !isfirstlevel ) {
        gevaluerror = true;
        cout << "ERROR (level of EXIT)" << endl;
        CleanTree( originhead );
        return false;
      } // if

      if ( ! Argumentnum( 0, 0, head->right, head->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      if ( Checkexit( head ) ) {
        gexit = true;
        CleanTree( originhead );
        return true;
      } // if

      CleanTree( originhead );
      return false;
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "let" ) == 0 ) {
      BTree runner1 = head->right;
      if ( !Argumentnum( 3, 9999, head, head->left->token, false ) ) {
        cout << "ERROR (LET format) : ";
        Preorder( head, false, 0 );
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if
        
      if ( runner1 != NULL && strcmp( runner1->type, "LEFT-PAREN" ) == 0
           && Islist2( runner1 ) ) { // ( let "(" 
        if ( strcmp( runner1->left->type, "NIL" ) == 0 ) {
        } // if
        else if ( strcmp( runner1->left->type, "LEFT-PAREN" ) == 0 
                  && Islist2( runner1->left ) ) { // ( let ( "("
          BTree runner0 = runner1->left;
          BTree runner2 = runner0->left; 
          // runner0->left = runner1->left, so it is not null
          if ( runner2 != NULL && strcmp( runner2->type, "LEFT-PAREN" ) == 0 ) { // ( let ( "("
            BTree runner3 = runner2->left;
            while ( runner0 != NULL && strcmp( runner0->type, "LEFT-PAREN" ) == 0 && 
                    runner2 != NULL && strcmp( runner2->type, "LEFT-PAREN" ) == 0 ) { // ( let ( "("

              if ( !Argumentnum( 2, 2, runner2, head->left->token, false ) ) {
                cout << "ERROR (LET format) : ";
                Preorder( head, false, 0 );
                gevaluerror = true;
                CleanTree( originhead );
                return false;
              } // if
            
              if ( runner3 == NULL || 
                   ( runner3 != NULL && strcmp( runner3->type, "SYMBOL" ) != 0 ) ) { // ( let ( ( "x" 
                cout << "ERROR (LET format) : ";
                Preorder( head, false, 0 );
                gevaluerror = true;
                CleanTree( originhead );
                return false;
              } // if
            
              runner0 = runner0->right;
              if ( runner0 != NULL && strcmp( runner0->type, "LEFT-PAREN" ) == 0 ) { // (let "("
                runner2 = runner0->left;
                if ( runner2 != NULL && strcmp( runner2->type, "LEFT-PAREN" ) == 0 
                     && Islist2( runner2 ) ) // ( let ( "("
                  runner3 = runner2->left;  // ( let ( ( "x" 
                else {
                  cout << "ERROR (LET format) : ";
                  Preorder( head, false, 0 );
                  gevaluerror = true;
                  CleanTree( originhead );
                  return false;
                } // else  
              } // if
            
            } // while
          } // if
          else {
            cout << "ERROR (LET format) : ";
            Preorder( head, false, 0 );
            gevaluerror = true;
            CleanTree( originhead );
            return false;         
          } // else
        } // else if
        else {
          cout << "ERROR (LET format) : ";
          Preorder( head, false, 0 );
          gevaluerror = true;
          CleanTree( originhead );
          return false;         
        } // else

      } // if
      else {
        cout << "ERROR (LET format) : ";
        Preorder( head, false, 0 );
        gevaluerror = true;
        CleanTree( originhead );
        return false;           
      } // else

      // check error
      NewdefinedArea( nowdeList, nownode );
      BTree runner = head->right->left;
      SStr command = "define";
      BTree commandhead = NULL;
      if ( strcmp( head->right->left->type, "NIL" ) != 0 ) {
        strcpy( command, "define" );
        BeCommendhead( commandhead, command );
        gnotlocalarea = false;
        while ( runner != NULL && strcmp( runner->type, "NIL" ) != 0 ) {
          CopyTree( runner->left, commandhead->right ); // ( define "a" .... )
          
          if ( !Preorder_Evalu( NULL, commandhead, true, 
                                nowdeList, nowdeList->list, infunction, true,
                                false, false ) ) { // define
            CleanTree( originhead );
            gnoreturnvalue = false;
            return false; 
          } // if

          CleanTree( commandhead->right );
          runner = runner->right;
        } // while
      }  // if

      // Checkdelist( nowdeList, nowdeList->list );
      BTree temp = NULL;
      CopyTree( head->right->right, temp );
      strcpy( command, "begin" );
      BeCommendhead( commandhead, command );
      CopyTree( temp, commandhead->right );
      if ( !Preorder_Evalu( NULL, commandhead, false, nowdeList, nowdeList->list, infunction, true,
                            false, false ) ) { // begin
        CleanTree( originhead );
        gnoreturnvalue = false;
        return false;
      } // if

      if ( strcmp( head->right->left->type, "NIL" ) != 0 ) 
        DeleteTopAreavariableList( nowdeList );

      CopyTree( commandhead, head );
      CleanTree( temp );
      CleanTree( commandhead );
      gdontprint = false;
      CleanTree( originhead );
      return true;
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->token, "lambda" ) == 0 ) {
      if ( !Islist2( head ) || !Argumentnum( 3, 9999, head, head->left->token, false ) ) {
        cout << "ERROR (lambda format)" << endl;
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      if ( head->right != NULL && strcmp( head->right->type, "LEFT-PAREN" ) == 0 )  { 
        BTree runner1 = head->right->left;
        if ( runner1 != NULL && strcmp( runner1->type, "LEFT-PAREN" ) == 0
             && Islist2( runner1 ) ) { // ( lambda "("

          BTree runner2 = runner1->left;  // ( lambda ( "x"
          while ( runner1 != NULL && strcmp( runner1->type, "LEFT-PAREN" ) == 0 ) { // ( lambda "("
            if ( strcmp( runner2->type, "SYMBOL" ) != 0 ) { // ( lambda ( "x"
              cout << "ERROR (lambda format)" << endl;
              gevaluerror = true;
              CleanTree( originhead );
              return false;
            } // if

            runner1 = runner1->right;
            if ( runner1 != NULL && strcmp( runner1->type, "LEFT-PAREN" ) == 0 )
              runner2 = runner1->left;
          } // while
        } // if
        else if ( runner1 != NULL && strcmp( runner1->type, "NIL" ) == 0 ) { // ( lambda "()"
        } // else if
        else {
          cout << "ERROR (lambda format)" << endl;
          gevaluerror = true;
          CleanTree( originhead );
          return false;
        } // else
      } // if
      else {
        cout << "ERROR (lambda format)" << endl;
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // else


      // check error
      BTree statement = new STree();  // deal the lambda's statement  start)
      strcpy( statement->token, "(" );
      strcpy( statement->type, "LEFT-PAREN" );
      statement->left = NULL;
      statement->right = NULL;
      BTree runner = head->right->right;
      for ( BTree runner2 = statement ; runner != NULL && strcmp( runner->type, "NIL" ) != 0 ; 
            runner = runner->right, runner2 = runner2->right ) {
        /*
        if ( runner2->left == NULL ) {
          runner2->left = new STree();
          strcpy( runner2->left->token, "(" );
          strcpy( runner2->left->type, "LEFT-PAREN" );   
          runner2->left->right = NULL;
          runner2->left->left = NULL;
        } // if
        */

        CopyTree( runner->left, runner2->left );
        if ( runner->right != NULL && strcmp( runner->right->type, "NIL" ) != 0 ) {
          runner2->right = new STree();
          strcpy( runner2->right->token, "(" );
          strcpy( runner2->right->type, "LEFT-PAREN" );
          runner2->right->right = NULL;
          runner2->right->right = NULL;
        } // if
      } // for                    // deal the lambda's statement  end )

      BTree parameter = new STree();
      parameter->left = NULL;
      parameter->right = NULL;
      BTree runner2 = parameter;
      if ( strcmp( head->right->left->type, "NIL" ) != 0 ) {
        strcpy( parameter->token, "(" );
        strcpy( parameter->type, "LEFT-PAREN" );
        for ( runner = head->right->left ; runner != NULL && strcmp( runner->type, "NIL" ) != 0 ;
              runner = runner->right ) {
          runner2->left = new STree();
          strcpy( runner2->left->token, "(" );
          strcpy( runner2->left->type, "LEFT-PAREN" );
          runner2->left->right = NULL;
          CopyTree( runner->left, runner2->left->left );
          if ( runner->right != NULL && strcmp( runner->right->type, "NIL" ) != 0 ) {
            runner2->right = new STree();
            strcpy( runner2->right->token, "(" );
            strcpy( runner2->right->type, "LEFT-PAREN" );
            runner2 = runner2->right;
            runner2->right = NULL;
          } // if
        } // for
      } // if
      else {
        strcpy( parameter->token, "nil" );
        strcpy( parameter->type, "NIL" );
      } // else

      NewTemplambda( parameter, statement );
      CleanTree( parameter );
      CleanTree( statement );
      /* 
      for ( TemplambdaList runner2 = gtemplambdaList ; runner2 != NULL ; runner2 = runner2->next ) {
        cout << endl << "name: " << runner2->name << endl;
        cout << "parameter: ";
        Preorder( runner2->parameter, false, 0 );           // !!use for checking gtemplambdaList )
        cout << "statement: " ; 
        Preorder( runner2->statement, false, 0  );
      } // for
      */ 

      SStr address = "";
      strcpy( address, head->address ); 
      CleanTree( head );
      head = new STree();
      /*
        if ( strcmp( address, "" ) != 0 && CheckifisFunctioninDeList( address, gdeList ) ) {
        SStr tokenname = "#<procedure ";
        strcat( tokenname, address );
        strcat( tokenname, ">" );
        strcpy( head->token, tokenname );
      } // if
      else {
      */
      strcpy( head->token, "#<procedure lambda>" );
      // } // else

      strcpy( head->type, "FUNCTION" );
      strcpy( head->address, gtemplambdaList->name ) ;
      head->left = NULL;
      head->right = NULL;
      CleanTree( originhead );
      return true;
    } // else if
    else if ( !head->left->hasquote && strcmp( head->left->type, "FUNCTION" ) == 0 ) {
      SStr name1;
      int j = 0;
      for ( int i = 12 ; i < strlen( head->left->token )-1 ; ++j, ++i )
        name1[j] = head->left->token[i];
      name1[j] = '\0';

      if ( !Islist2( head ) ) {
        cout << "non-list in function" << endl; 
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      // check error
      SStr address;
      strcpy( address, head->left->address ) ;
      TemplambdaList runner = gtemplambdaList;
      for ( bool hasans = false ; !hasans && runner != NULL ; ) {  // find the procedure   store by runner)
        if ( strcmp( runner->name, address ) != 0 ) {
          runner = runner->next;
        } // if
        else
          hasans = true;
      } // for

      int num = 1;
      BTree cacunum = runner->parameter; 
      if ( strcmp( cacunum->type, "NIL" ) == 0 )
        num = 1;
      else {
        for ( ; cacunum != NULL && strcmp( cacunum->type, "NIL" ) != 0 ; cacunum = cacunum->right ) {
          ++num;
        } // for
      } // else

      SStr name2 = "";
      ChecknameinDeList( originhead->left->token, name2, nowdeList, nownode );
      if ( CheckifisFunctioninDeList( name2, nowdeList, nownode ) )
        strcpy( originhead->left->token, name2 );
      else
        strcpy( originhead->left->token, "lambda" );
      if ( !Argumentnum( num, num, head, originhead->left->token, true ) ) {
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      BTree caculate = head->right; 
      for ( int i = 0 ; caculate != NULL && strcmp( caculate->type, "NIL" ) != 0 ; 
            caculate = caculate->right, ++i ) {  
        // 走訪左子樹並檢查有無 symbol or instruction需要計算 ) 
        if ( caculate->left != NULL && 
             ( strcmp( caculate->left->token, "(" ) == 0 || strcmp( caculate->left->type, "SYMBOL" ) == 0 ) 
             && !Preorder_Evalu( caculate, caculate->left, false, nowdeList, nownode, infunction, true,
                                 false, false ) ) {
          gevaluerror = true;
          gnoreturnvalue = false;
          CleanTree( originhead );
          return false;
        } // if


        if ( caculate->left != NULL && strcmp( caculate->left->type, "LEFT-PAREN" ) == 0 ) { 
          // if the head of the result is left-paren )
          BTree commandhead = NULL;
          SStr command = "quote"; 
          BeCommendhead( commandhead, command );
          commandhead->right = new STree();
          strcpy( commandhead->right->token, "(" );
          strcpy( commandhead->right->type, "LEFT-PAREN" );
          CopyTree( caculate->left, commandhead->right->left );
          commandhead->right->right = NULL;
          CopyTree( commandhead, caculate->left );
          CleanTree( commandhead );
        } // if

      } // for



      BTree commandhead = NULL;
      SStr command = "let"; 
      BeCommendhead( commandhead, command );
      BTree statement = NULL;
      BTree parameterbuilder = NULL;
      CopyTree( runner->parameter, parameterbuilder );
      CopyTree( runner->statement, statement );
      BTree runparameter = parameterbuilder;
      if ( strcmp( runner->parameter->type, "NIL" ) != 0 ) {
        for ( BTree parametervalue = head->right ; 
              parametervalue != NULL && strcmp( parametervalue->type, "NIL" ) != 0 ;
              parametervalue = parametervalue->right, runparameter = runparameter->right ) {
          runparameter->left->right = new STree();
          strcpy( runparameter->left->right->token, "(" );
          strcpy( runparameter->left->right->type, "LEFT-PAREN" );
          CopyTree( parametervalue->left, runparameter->left->right->left );
          runparameter->left->right->right = NULL;
        } // for
      } // if

      /*
      Preorder( statement, false, 0 );
      system("pause");
      */
      
      commandhead->right = new STree();
      strcpy( commandhead->right->token, "(" );
      strcpy( commandhead->right->type, "LEFT-PAREN" );
      CopyTree( parameterbuilder, commandhead->right->left );
      commandhead->right->right = NULL; 
      CopyTree( statement, commandhead->right->right );
      head = commandhead;
      // Preorder( head, false, 0 );
      // system("pause");

      if ( !Preorder_Evalu( NULL, head, true, nowdeList, nownode, true, true,
                            false, false ) ) {
        if ( gnoreturnvalue && !infunction ) {
          if ( !isfirstlevel )
            cout << "ERROR (unbound parameter) : ";
          else 
            cout << "ERROR (no return value) : ";
          Preorder( originhead, false, 0 );
          gnoreturnvalue = false;
        } // if

        gevaluerror = true;
        gnoreturnvalue = false;
        CleanTree( originhead );
        return false;
      } // if

      CleanTree( originhead );
      CleanTree( commandhead );
      CleanTree( statement );
      CleanTree( parameterbuilder );
      gdontprint = false;
      return true;
      
    } // else if
    else  {
     // Error non function
      if ( !Islist2( head ) ) {
        cout << "ERROR (non-list) : ";
        Preorder( head, false, 0 );
        gevaluerror = true;
        CleanTree( originhead );
        return false;
      } // if

      cout << "ERROR (attempt to apply non-function) : ";
      Preorder( head->left, false, 0 );
      gevaluerror = true;
      CleanTree( originhead );
      return false;
    } // else
  } // if
  else if ( strcmp( head->type, "SYMBOL" ) == 0 ) {                    // 開頭不為"("的情況下 )
    SStr originname;
    strcpy( originname, head->token );
    // Checkdelist( maygotonext, maygotonextnode );
    // system("pause");
    if ( CheckifisFunctioninDeList( head->token, nowdeList, nownode ) ) {
      SStr tokenname = "#<procedure ";
      strcat( tokenname, head->token );
      strcat( tokenname, ">" );
      strcpy( head->address, head->token );
      strcpy( head->token, tokenname );
      strcpy( head->type, "PREFUNCTION" );
      gdontprint = false;
      return true;    
    } // if
    else if ( CheckifinDeList_S1( originname, head, nowdeList, nownode ) ) {
      // Checkdelist( maygotonext, maygotonextnode );
      // system("pause");
      // checkifinDeList_s1 will return the head & address of origin symbol )
      /*
      if ( maygotonext->next != NULL ) { // if we need to caculate a local symbol
        maygotonextnode = maygotonext->linker;
        maygotonext = maygotonext->next;
      } // if
      else {
        maygotonext = maygotonext;
        maygotonextnode = maygotonextnode->next;
      } // else

      if ( ( strcmp( head->token, "(" ) == 0 
             || strcmp( head->type, "SYMBOL" ) == 0 ) 
           && !Preorder_Evalu( linker, head, false, maygotonext, maygotonextnode ) )
        cout << "error in symbol";
      */
      gdontprint = false;
      return true;
    } // else if
    else if ( head->hasquote ) {
      gdontprint = false;
      return true;
    } // else if
    else if ( strcmp( head->token, "gdelist" ) == 0 ) {   // use for checking error
      AreavariableList runarea = gdeList;
      DefinitionList runner = NULL; 
      while ( runarea != NULL ) {
        runner = runarea->list;
        while ( runner != NULL ) {
          cout << "name: " << runner->name << "\thead :";
          Preorder( runner->head, false, 0 ); 
          runner = runner->next;
        } // while
        
        cout << endl;
        runarea = runarea->next;
      } // while
    } // else if
    else if ( strcmp( head->token, "gturnon" ) == 0 ) {
      gturnon = true;
      return true;
    } // else if
    else  {
      // unbound symbol
      cout << "ERROR (unbound symbol) : " << head->token << endl; 
      gevaluerror = true;
      return false;
    } // else
  } // else if
  else {
    gdontprint = false;
    return true;
  } // else

  return false;
} // Preorder_Evalu()


int main() {
  SStr token, type;
  BTree head = NULL;
  BTree evaluTree = NULL ;
  BTree bnode = head ;          // where should the system build a new node )
  NewdefinedArea( gdeList, NULL );
  InstructionDefinition();   // add instruction's definition ) 
  cout << "Welcome to OurScheme!" << endl;
  char ch;
  int i = 0;
  if ( !gexit && !gArriveEOF ) {         
    GetNextToken( token );
    if ( !gArriveEOF )
      Type( token, type );
    while ( !gArriveEOF && !gexit ) {          // 使用while是因為假如 isSexp為FALSE時 仍需要繼續運作 ) 
      if ( !gerror && IsSexp( head, bnode, token, type, false, false ) ) { // no syntax error )
        cout << endl << "> ";
        if ( Checkexit( head ) ) {
          gexit = true;
        } // if
        else {     // evaluate the Tree )
          CopyTree( head, evaluTree );
          if ( strcmp( evaluTree->token, "(" ) == 0 || strcmp( evaluTree->type, "SYMBOL" ) == 0 ) {
            // 輸入的S-expr需要計算 )
            if ( Preorder_Evalu( NULL, evaluTree, true, gdeList, gdeList->list, false, true,
                                 false, false ) 
                 && !gdontprint ) { 
              // caculate non error )
              Preorder( evaluTree, false, 0 ) ;
            } // if
            else 
              gevaluerror = false;
          } // if
          else  { // ( 輸入為atom ) 
            Preorder( evaluTree, false, 0 ) ;
          } // else
        } // else

 
        gdontprint = false;
        gevaluerror = false;
        // if (  gevaluerror ) 
        // system( "pause" );
        if ( !gexit ) { 
          strcpy( glasttype, "" );
          gline = 1;
          gcolumn = 0;
          while ( cin.peek() == ' ' && cin.peek() != EOF ) {
            scanf( "%c", &ch );
            ++gcolumn;
            if ( cin.peek() == ';' ) {
              while ( cin.peek() != '\n' && cin.peek() != EOF ) {
                scanf( "%c", &ch );
              } // while
            } // if
          } // while

          if ( cin.peek() == '\n' ) {
            scanf( "%c", &ch );
            gcolumn = 0; 
          } // if

          GetNextToken( token );
          if ( !gArriveEOF )
            Type( token, type );
          gnotlocalarea = true;
          CleanTree( head );
          CleanTree( evaluTree ); 
          CleanWaitingRP( gNeedRP );
          bnode = NULL;
          gNeedRP = NULL;
          CleanTree( evaluTree );
          gdontprint = false; 
        } // if
      } // if     符合文法的情況下) 
      else if ( !gArriveEOF ) {               // 出現error的情況)
        strcpy( glasttype, "" );
        gline = 1;
        gcolumn = 0;
        while ( cin.peek() != '\n' && cin.peek() != EOF ) {
          scanf( "%c", &ch );
          // ++gcolumn;
        } // while

        if ( cin.peek() == '\n' ) {
          scanf( "%c", &ch );
          // ++gcolumn;
        } // if

        gerror = false;
        gnotlocalarea = true;
        GetNextToken( token );
        Type( token, type );
        CleanTree( head );
        CleanWaitingRP( gNeedRP );
        bnode = NULL;
        gNeedRP = NULL;
      } // else if  ERROR發生時此處error不包含EOF )
    } // while
  } // if

  cout << endl << "Thanks for using OurScheme!" ;
} // main() 

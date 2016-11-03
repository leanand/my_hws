package edu.binghamton.cs571;

public class UglyRegexpParser {

  Token _lookahead;
  Scanner _scanner;

  UglyRegexpParser(Scanner scanner) {
    _scanner = scanner;
    _lookahead = _scanner.nextToken();
  }


  /** parse a sequence of lines containing ugly-regexp's; for each
   *  ugly regexp print out the corresponding standard regexp.
   *  If there is an error, print diagnostic and continue with
   *  next line.
   */
  public void parse() {
    while (_lookahead.kind != Token.Kind.EOF) {
      try {
        String out = uglyRegexp();
        if (check(Token.Kind.NL)) System.out.println(out);
        match(Token.Kind.NL);
      }
      catch (ParseException e) {
        System.err.println(e.getMessage());
        while (_lookahead.kind != Token.Kind.NL) {
          _lookahead = _scanner.nextToken();
        }
        _lookahead = _scanner.nextToken();
      }
    }
  }

  /** Return standard syntax regexp corresponding to ugly-regexp
   *  read from _scanner.
   */
  private String uglyRegexp() {
    return concatMatch();
  }
 
  /** Concat
   *    :  Alter ConcatRest
   *    ;
   */
   
  private String concatMatch(){
    String alter = alterMatch();
    return concatRestMatch(alter);
  }
  
  /** ConcatRest
   *    :  DOT_OP Alter ConcatRest
   *    |  //EMPTY 
   *    ;
   */

  private String concatRestMatch(String prevOut){
    if(check(Token.Kind.CHAR, ".") == true){
      match(Token.Kind.CHAR, ".");
      String alter = alterMatch();
      return concatRestMatch("(" + prevOut + alter + ")" );
    }else{
      return prevOut;
    }
  }

  /** Alter
   *    :  Closure AlterRest
   *    ;
   */

  private String alterMatch(){
    String closure = closureMatch();
    return alterRestMatch(closure);
  }
  
  /** AlterRest
   *    :  PLUS_OP Closure AlterRest
   *    |  //EMPTY
   *    ;
   */
  private String alterRestMatch(String prevValue){
    if(check(Token.Kind.CHAR, "+") == true){
      match(Token.Kind.CHAR, "+");
      String closure = closureMatch();
      return alterRestMatch("(" + prevValue + "|" + closure + ")");
    }else{
      return prevValue;
    }
  }
  
  /** Closure
   *    : STAR_OP Closure
   *    | CHARS_OP OPEN_PAREN CharSet CLOSE_PAREN
   *    | OPEN_PAREN Concat CLOSE_PAREN
   *    ;
   */

  private String closureMatch(){
    if(check(Token.Kind.CHAR, "*") == true){
      match(Token.Kind.CHAR, "*");
      return closureMatch() + "*";
    }else if(check(Token.Kind.CHARS)){
      match(Token.Kind.CHARS);
      match(Token.Kind.CHAR, "(");
      String charSet = charSetMatch();
      match(Token.Kind.CHAR, ")");
      return "[" + charSet + "]"; 
    }else{
      match(Token.Kind.CHAR, "(");
      String concat = concatMatch();
      match(Token.Kind.CHAR, ")");
      return "(" + concat + ")";
    }
  }
  
  /** CharSet
   *   : Char COMMA_OP CharSet
   *   | Char
   *   ;
   */
  
  private String charSetMatch(){
    String charSet = quote(_lookahead.lexeme);
    match(Token.Kind.CHAR);
    if(check(Token.Kind.CHAR, ",") == true){
      match(Token.Kind.CHAR, ",");
      charSet = charSet + charSetMatch();
    }
    return charSet;
  }




  //Utility functions which may be useful for parsing or translation

  /** Return s with first char escaped using a '\' if it is
   * non-alphanumeric.
   */
  private static String quote(String s) {
    return (Character.isLetterOrDigit(s.charAt(0))) ? s : "\\" + s;
  }

  /** Return true iff _lookahead.kind is equal to kind. */
  private boolean check(Token.Kind kind) {
    return check(kind, null);
  }

  /** Return true iff lookahead kind and lexeme are equal to
   *  corresponding args.  Note that if lexeme is null, then it is not
   *  used in the match.
   */
  private boolean check(Token.Kind kind, String lexeme) {
    return (_lookahead.kind == kind &&
            (lexeme == null || _lookahead.lexeme.equals(lexeme)));
  }

  /** If lookahead kind is equal to kind, then set lookahead to next
   *  token; else throw a ParseException.
   */
  private void match(Token.Kind kind) {
    match(kind, null);
  }

  /** If lookahead kind and lexeme are not equal to corresponding
   *  args, then set lookahead to next token; else throw a
   *  ParseException.  Note that if lexeme is null, then it is
   *  not used in the match.
   */
  private void match(Token.Kind kind, String lexeme) {
    if (check(kind, lexeme)) {
      _lookahead = _scanner.nextToken();
    }
    else {
      String expected = (lexeme == null) ? kind.toString() : lexeme;
      String message = String.format("%s: syntax error at '%s', expecting '%s'",
                                     _lookahead.coords, _lookahead.lexeme,
                                     expected);
      throw new ParseException(message);
    }
  }

  private static class ParseException extends RuntimeException {
    ParseException(String message) {
      super(message);
    }
  }


  /** main program: parses and translates ugly-regexp's contained in
   *  the file specified by it's single command-line argument.
   */
  public static void main(String[] args) {
    if (args.length != 1) {
      System.err.format("usage: java %s FILENAME\n",
                        UglyRegexpParser.class.getName());
      System.exit(1);
    }
    Scanner scanner =
      ("-".equals(args[0])) ? new Scanner() : new Scanner(args[0]);
    (new UglyRegexpParser(scanner)).parse();
  }


}

{ if-then-else, expressoes booleanas e read }
program Maior;
var a, b : integer;
    ok : boolean;
begin
  read(a, b);
  ok := (a > b) and not (a = 0);
  if ok then
    writeln('a e maior')
  else if a = b then
    writeln('iguais')
  else
    writeln('b e maior');
end.

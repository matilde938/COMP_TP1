// ciclo while: soma de 1 a n
program Soma;
var n, i, s : integer;
begin
  readln(n);
  i := 1;
  s := 0;
  while i <= n do
  begin
    s := s + i;
    i := i + 1
  end;
  writeln('soma = ', s)
end.

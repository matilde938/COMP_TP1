(* maiusculas, plicas dentro de strings, writeln() e dangling else *)
PROGRAM Testes;
VAR x : Integer;
BEGIN
  x := 10;
  WriteLn('It''s ok');
  writeln();
  if x > 0 then
    if x > 5 then writeln('grande')
    else writeln('pequeno')
END.

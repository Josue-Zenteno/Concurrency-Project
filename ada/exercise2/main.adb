with pkg_procedure; use pkg_procedure;
with pkg_task; use pkg_task;

procedure main is
    
    a : Integer;

    begin
        
        loop 
            
            tarea_t.EstadoConsulta;
            Leer_Entero(a);
            
            if(a = 0) then
                exit;
            end if;

            tarea_t.EsPar(a);
        
        end loop;

        abort tarea_t;

end main;
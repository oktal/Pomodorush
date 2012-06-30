CREATE TRIGGER on_delete_todo
AFTER DELETE ON todo
FOR EACH ROW 
BEGIN 

DELETE FROM pomodoro_state WHERE todo_id=OLD.id;

END
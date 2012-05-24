-- Creator:       MySQL Workbench 5.2.38/ExportSQLite plugin 2009.12.02
-- Author:        octal
-- Caption:       New Model
-- Project:       Name of the project
-- Changed:       2012-05-24 14:50
-- Created:       2012-05-22 16:11
PRAGMA foreign_keys = OFF;

-- Schema: mydb
BEGIN;
CREATE TABLE "activity"(
  "id" INTEGER PRIMARY KEY NOT NULL,
  "date" DATE,
  "type" TEXT,
  "description" TEXT,
  "estimation" INTEGER,
  "urgent" INTEGER
);
CREATE TABLE "todo"(
  "id" INTEGER PRIMARY KEY NOT NULL,
  "date" DATE,
  "type" TEXT,
  "description" INTEGER,
  "estimation" INTEGER,
  "done" INTEGER,
  "pomodoro_done" INTEGER,
  "urgent" INTEGER
);
CREATE TABLE "todo_reestimation"(
  "todo_id" INTEGER NOT NULL,
  "reestimation" INTEGER,
  CONSTRAINT "fk_todo_estimation_Todo"
    FOREIGN KEY("todo_id")
    REFERENCES "todo"("id")
);
CREATE INDEX "todo_reestimation.fk_todo_estimation_Todo" ON "todo_reestimation"("todo_id");
CREATE TABLE "interruption"(
  "todo_id" INTEGER NOT NULL,
  "interruption_type" INTEGER,
  "reason" TEXT,
  CONSTRAINT "fk_interruption_Todo1"
    FOREIGN KEY("todo_id")
    REFERENCES "todo"("id")
);
CREATE INDEX "interruption.fk_interruption_Todo1" ON "interruption"("todo_id");
CREATE TABLE "record"(
  "todo_id" INTEGER NOT NULL,
  CONSTRAINT "fk_record_todo1"
    FOREIGN KEY("todo_id")
    REFERENCES "todo"("id")
);
CREATE INDEX "record.fk_record_todo1" ON "record"("todo_id");
COMMIT;

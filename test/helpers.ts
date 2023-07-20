import fs from "node:fs";
import os from "node:os";
import crypto from "node:crypto";
import path from "node:path";

export function getTemporaryFilePath({
  prefix,
  suffix
}: {
  prefix?: string;
  suffix: string;
}) {
  const name = crypto.randomUUID();
  return path.join(os.tmpdir(), (prefix || "") + name + suffix);
}

export function removeFile(filename: string) {
  if (fs.existsSync(filename)) {
    fs.unlinkSync(filename);
  } else {
    //Show in red
    console.log("File " + filename + " not found, so not deleting.");
  }
}

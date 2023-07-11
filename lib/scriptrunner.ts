import vm from "vm";
import os from "os";
import path from "path";
const doDebug = false;

function getTemporaryFilePath({
  prefix,
  suffix
}: {
  prefix: string;
  suffix: string;
}) {
  return path.join(os.tmpdir(), prefix, suffix);
}
export class ScriptRunner {
  public env: Record<string, any>;

  constructor(envparam: any) {
    console.log("Constructing a ScriptRunner");
    this.env = {
      print: () => {
        this.env.console.log.apply(this.env.console, arguments);
      },

      fnum: (value: number, width: number, prec: number) => {
        const a = value.toFixed(prec);
        const s = "             " + a;
        return s.slice(s.length - width);
      },

      logs: [],

      console: {
        log: () => {
          /// console.log.apply(console,arguments);
          this.env.logs.push(arguments);
        }
      },

      eval: () => {
        throw new Error("eval is forbidden");
      },
      require: () => {
        throw new Error("require is forbidden");
      },
      setTimeout: () => {
        throw new Error("setTimeout is forbidden");
      },
      setInterval: () => {
        throw new Error("setInterval is forbidden");
      },
      error: null
    };

    // extend default env object with env parameters
    Object.keys(envparam).forEach((p) => {
      if (envparam.hasOwnProperty(p)) {
        this.env[p] = envparam[p];
      }
    });
  }

  run(
    code: string,
    done_callback: () => void,
    error_callback: (err: Error) => void
  ) {
    const settings = {
      prefix: "script",
      suffix: ".bar"
    };

    const log = this.env.console.log;

    const filename = getTemporaryFilePath(settings);
    (async () => {
      try {
        let result = await vm.runInNewContext(code, this.env, filename);
        done_callback();
      } catch (_err) {
        const err = _err as Error;
        console.trace(
          "\n-------------------------------- EXCEPTION CAUGHT -----------------"
        );

        log("transaction ended with an error", err.message);
        // log("error string = ", err.toString());
        log("error stack  = ", err.stack);
        error_callback(err);
      }
    })();
  }
}

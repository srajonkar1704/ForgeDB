import { motion } from "framer-motion";
import {
  ScanText,
  GitBranch,
  Cpu,
  Database,
  Layers,
  HardDrive,
  Table2
} from "lucide-react";

const icons = {
  tokenizer: ScanText,
  parser: GitBranch,
  executor: Cpu,
  heapfile: Database,
  pager: Layers,
  page: HardDrive,
  results: Table2,
  create: Database,
  insert: Cpu,
  select: Table2,
  disk: HardDrive
};

export default function ExecutionPipeline({
  stages,
  activeStage,
  completedStages
}) {

  return (
    <div className="execution-pipeline">

      <div className="pipeline-track">

        {stages.map((stage, index) => {

          const Icon = icons[stage.id];

          const active = activeStage === index;
          const completed = completedStages.includes(index);

          return (
            <div className="pipeline-step" key={stage.id}>

              <motion.div
                className={`pipeline-card ${
                  active ? "active" : ""
                } ${completed ? "completed" : ""}`}

                animate={{
                  scale: active ? 1.08 : 1,
                  opacity: active || completed ? 1 : 0.45
                }}

                transition={{ duration: 0.35 }}
              >

                <Icon size={25} />

                <h4>{stage.title}</h4>

                <p>{stage.description}</p>

                {active && (
                  <motion.div
                    className="processing-indicator"
                    initial={{ width: 0 }}
                    animate={{ width: "100%" }}
                    transition={{ duration: 0.8 }}
                  />
                )}

              </motion.div>

              {index !== stages.length - 1 && (
                <div className="pipeline-connector">

                  <motion.div
                    className="connector-pulse"
                    initial={{ scaleX: 0 }}
                    animate={{
                      scaleX: completed ? 1 : 0
                    }}
                    transition={{ duration: 0.5 }}
                  />

                  <span>→</span>

                </div>
              )}

            </div>
          );
        })}

      </div>

    </div>
  );
}
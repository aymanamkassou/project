"use client";

import { Card, CardHeader, CardContent } from "@/components/ui/card";
import { Button } from "@/components/ui/button";
import { Slider } from "@/components/ui/slider";
import { Play, Pause, SkipBack, SkipForward } from "lucide-react";

// Base Node interface matching C++ Node class
interface BaseNode {
  id: string;
  lat: number;
  lng: number;
  type: number; // 0 for airports, 1 for waypoints
}

// Airport interface extending BaseNode
interface Airport extends BaseNode {
  name: string;
  city: string;
  country: string;
  elevation: number;
}

// Waypoint interface extending BaseNode
interface Waypoint extends BaseNode {
  countryCode: string;
  countryName: string;
}

type Node = Airport | Waypoint;

interface Edge {
  from: string;
  to: string;
  distance: number;
}

interface PathResult {
  path: string[];
  totalDistance: number;
  steps: AlgorithmStep[];
}

interface AlgorithmStep {
  currentNode: string;
  visitedNodes: string[];
  frontier: string[];
  distances: Record<string, number | "∞">;
  previousNodes: Record<string, string>;
}

interface AlgorithmVisualizerProps {
  pathResult: PathResult;
  currentStepIndex: number;
  onStepChange: (index: number) => void;
  isAnimating: boolean;
  onAnimationToggle: (value: boolean) => void;
}

export default function AlgorithmVisualizer({
  pathResult,
  currentStepIndex,
  onStepChange,
  isAnimating,
  onAnimationToggle,
}: AlgorithmVisualizerProps) {
  const currentStep = pathResult.steps[currentStepIndex];

  return (
    <Card>
      <CardHeader>
        <h2 className="text-2xl font-bold">Algorithm Progress</h2>
      </CardHeader>
      <CardContent className="space-y-4">
        <div className="space-y-2">
          <p className="text-sm font-medium">Current Node: {currentStep.currentNode}</p>
          <p className="text-sm">
            Distance: {pathResult.totalDistance.toFixed(2)} nautical miles
          </p>
        </div>

        <div className="space-y-4">
          <div className="flex items-center justify-between gap-2">
            <Button
              variant="outline"
              size="icon"
              onClick={() => onStepChange(0)}
              disabled={currentStepIndex === 0}
            >
              <SkipBack className="h-4 w-4" />
            </Button>

            <Button
              variant="outline"
              size="icon"
              onClick={() => onAnimationToggle(!isAnimating)}
            >
              {isAnimating ? (
                <Pause className="h-4 w-4" />
              ) : (
                <Play className="h-4 w-4" />
              )}
            </Button>

            <Button
              variant="outline"
              size="icon"
              onClick={() => onStepChange(pathResult.steps.length - 1)}
              disabled={currentStepIndex === pathResult.steps.length - 1}
            >
              <SkipForward className="h-4 w-4" />
            </Button>
          </div>

          <Slider
            value={[currentStepIndex]}
            min={0}
            max={pathResult.steps.length - 1}
            step={1}
            onValueChange={(value) => onStepChange(value[0])}
          />
        </div>

        <div className="space-y-2">
          <h3 className="text-sm font-medium">Path</h3>
          <div className="flex flex-wrap gap-2">
            {pathResult.path.map((nodeId, index) => (
              <div
                key={nodeId}
                className="flex items-center text-sm"
              >
                <span className="font-mono">{nodeId}</span>
                {index < pathResult.path.length - 1 && (
                  <span className="mx-2">→</span>
                )}
              </div>
            ))}
          </div>
        </div>
      </CardContent>
    </Card>
  );
}
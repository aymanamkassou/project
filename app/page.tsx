"use client";

import { useEffect, useState, useCallback } from "react";
import { Plane, Loader2 } from "lucide-react";
import FlightControls from "@/components/FlightControls";
import AlgorithmVisualizer from "@/components/AlgorithmVisualizer";
import { Button } from "@/components/ui/button";
import { toast } from "sonner";
import dynamic from 'next/dynamic';

const AirportMap = dynamic(() => import("@/components/AirportMap"), { ssr: false });

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

interface GraphData {
  nodes: Node[];
  edges: Edge[];
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

export default function Home() {
  const [graphData, setGraphData] = useState<GraphData | null>(null);
  const [selectedStart, setSelectedStart] = useState<string>("");
  const [selectedEnd, setSelectedEnd] = useState<string>("");
  const [algorithm, setAlgorithm] = useState<"dijkstra" | "bfs">("dijkstra");
  const [pathResult, setPathResult] = useState<PathResult | null>(null);
  const [isLoading, setIsLoading] = useState(false);
  const [currentStepIndex, setCurrentStepIndex] = useState(0);
  const [isAnimating, setIsAnimating] = useState(false);

  useEffect(() => {
    fetchGraphData();
  }, []);

  const fetchGraphData = async () => {
    try {
      const response = await fetch("/api/graph");
      const data = await response.json();
      setGraphData(data);
    } catch (error) {
      toast.error("Failed to load airport data");
    }
  };

  const findPath = async () => {
    if (!selectedStart || !selectedEnd) {
      toast.error("Please select both airports");
      return;
    }
  
    setIsLoading(true);
    try {
      const response = await fetch("/api/path", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({
          start: selectedStart,
          end: selectedEnd,
          algorithm,
        }),
      });
      
      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }
  
      const result = await response.json();
      const sanitizedResult = JSON.parse(
        JSON.stringify(result).replace(/"\s*:\s*inf/g, ': null')
      );
      
      setPathResult(sanitizedResult);
      setCurrentStepIndex(0);
      setIsAnimating(true);
    } catch (error) {
      toast.error(error instanceof Error ? error.message : "Failed to calculate route");
    } finally {
      setIsLoading(false);
    }
  };

  const clearFlight = () => {
    setSelectedStart("");
    setSelectedEnd("");
    setPathResult(null);
    setCurrentStepIndex(0);
    setIsAnimating(false);
  };

  const animateAlgorithm = useCallback(() => {
    if (!pathResult || currentStepIndex >= pathResult.steps.length - 1) {
      setIsAnimating(false);
      return;
    }

    const interval = setInterval(() => {
      setCurrentStepIndex((prev) => {
        if (prev >= pathResult.steps.length - 1) {
          clearInterval(interval);
          setIsAnimating(false);
          return prev;
        }
        return prev + 1;
      });
    }, 1000);

    return () => clearInterval(interval);
  }, [pathResult, currentStepIndex]);

  useEffect(() => {
    if (isAnimating) {
      animateAlgorithm();
    }
  }, [isAnimating, animateAlgorithm]);

  return (
    <main className="min-h-screen bg-gray-100 dark:bg-gray-900">
      <div className="container mx-auto px-4 py-8">
        <div className="flex flex-col gap-8">
          <header className="text-center">
            <h1 className="text-4xl font-bold text-gray-900 dark:text-white flex items-center justify-center gap-2">
              <Plane className="h-8 w-8" />
              IFR Route Pathfinding
            </h1>
            <p className="mt-2 text-gray-600 dark:text-gray-400">
              Visualize Dijkstra and BFS algorithms for finding optimal flight routes
            </p>
          </header>

          <div className="grid grid-cols-1 lg:grid-cols-3 gap-8">
            <div className="lg:col-span-2">
              <div className="bg-white dark:bg-gray-800 rounded-xl shadow-lg p-4">
                {graphData ? (
                  <AirportMap
                    nodes={graphData.nodes}
                    edges={graphData.edges}
                    pathResult={pathResult}
                    currentStep={pathResult?.steps[currentStepIndex]}
                    selectedStart={selectedStart}
                    selectedEnd={selectedEnd}
                  />
                ) : (
                  <div className="flex items-center justify-center h-[600px]">
                    <Loader2 className="h-8 w-8 animate-spin" />
                  </div>
                )}
              </div>
            </div>

            <div className="space-y-6">
              <FlightControls
                nodes={graphData?.nodes || []}
                selectedStart={selectedStart}
                selectedEnd={selectedEnd}
                onStartChange={setSelectedStart}
                onEndChange={setSelectedEnd}
                algorithm={algorithm}
                onAlgorithmChange={setAlgorithm}
                onFindPath={findPath}
                isLoading={isLoading}
              />

              {pathResult && (
                <AlgorithmVisualizer
                  pathResult={pathResult}
                  currentStepIndex={currentStepIndex}
                  onStepChange={setCurrentStepIndex}
                  isAnimating={isAnimating}
                  onAnimationToggle={setIsAnimating}
                />
              )}

              <Button
                variant="destructive"
                className="w-full"
                onClick={clearFlight}
              >
                Clear Flight
              </Button>
            </div>
          </div>
        </div>
      </div>
    </main>
  );
}
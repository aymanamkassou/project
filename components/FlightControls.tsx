"use client";

import {
  Select,
  SelectContent,
  SelectItem,
  SelectTrigger,
  SelectValue,
} from "@/components/ui/select";
import { Button } from "@/components/ui/button";
import { Card, CardHeader, CardContent } from "@/components/ui/card";
import { Loader2 } from "lucide-react";

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

interface FlightControlsProps {
  nodes: Node[];
  selectedStart: string;
  selectedEnd: string;
  onStartChange: (value: string) => void;
  onEndChange: (value: string) => void;
  algorithm: string;
  onAlgorithmChange: (value: "dijkstra" | "bfs") => void;
  onFindPath: () => void;
  isLoading: boolean;
}

export default function FlightControls({
  nodes,
  selectedStart,
  selectedEnd,
  onStartChange,
  onEndChange,
  algorithm,
  onAlgorithmChange,
  onFindPath,
  isLoading,
}: FlightControlsProps) {
  // Filter only airports (type 0) and sort by ID
  const airportOptions = nodes
    .filter((node): node is Airport => node.type === 0)
    .sort((a, b) => a.id.localeCompare(b.id));

  return (
    <Card>
      <CardHeader>
        <h2 className="text-2xl font-bold">Flight Controls</h2>
      </CardHeader>
      <CardContent className="space-y-4">
        <div className="space-y-2">
          <label className="text-sm font-medium">Departure Airport</label>
          <Select value={selectedStart} onValueChange={onStartChange}>
            <SelectTrigger>
              <SelectValue placeholder="Select departure" />
            </SelectTrigger>
            <SelectContent>
              {airportOptions.map((airport) => (
                <SelectItem key={airport.id} value={airport.id}>
                  {airport.id} - {airport.name}, {airport.city}
                </SelectItem>
              ))}
            </SelectContent>
          </Select>
        </div>

        <div className="space-y-2">
          <label className="text-sm font-medium">Arrival Airport</label>
          <Select value={selectedEnd} onValueChange={onEndChange}>
            <SelectTrigger>
              <SelectValue placeholder="Select arrival" />
            </SelectTrigger>
            <SelectContent>
              {airportOptions.map((airport) => (
                <SelectItem key={airport.id} value={airport.id}>
                  {airport.id} - {airport.name}, {airport.city}
                </SelectItem>
              ))}
            </SelectContent>
          </Select>
        </div>

        <div className="space-y-2">
          <label className="text-sm font-medium">Pathfinding Algorithm</label>
          <Select
            value={algorithm}
            onValueChange={(value: "dijkstra" | "bfs") => onAlgorithmChange(value)}
          >
            <SelectTrigger>
              <SelectValue />
            </SelectTrigger>
            <SelectContent>
              <SelectItem value="dijkstra">Dijkstras Algorithm</SelectItem>
              <SelectItem value="bfs">Breadth-First Search</SelectItem>
            </SelectContent>
          </Select>
        </div>

        <Button
          className="w-full"
          onClick={onFindPath}
          disabled={isLoading || !selectedStart || !selectedEnd}
        >
          {isLoading ? (
            <>
              <Loader2 className="mr-2 h-4 w-4 animate-spin" />
              Finding Route...
            </>
          ) : (
            "Find Route"
          )}
        </Button>
      </CardContent>
    </Card>
  );
}
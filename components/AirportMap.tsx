"use client";

import { useEffect } from "react";
import { MapContainer, TileLayer, Marker, Popup, Polyline, Circle } from "react-leaflet";
import L from "leaflet";
import "leaflet/dist/leaflet.css";

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

interface AirportMapProps {
  nodes: Node[];
  edges: Edge[];
  pathResult: PathResult | null;
  currentStep: AlgorithmStep | undefined;
  selectedStart: string;
  selectedEnd: string;
}

export default function AirportMap({
  nodes,
  edges,
  pathResult,
  currentStep,
  selectedStart,
  selectedEnd,
}: AirportMapProps) {
  useEffect(() => {
    delete (L.Icon.Default.prototype as any)._getIconUrl;
    L.Icon.Default.mergeOptions({
      iconRetinaUrl: "https://cdnjs.cloudflare.com/ajax/libs/leaflet/1.7.1/images/marker-icon-2x.png",
      iconUrl: "https://cdnjs.cloudflare.com/ajax/libs/leaflet/1.7.1/images/marker-icon.png",
      shadowUrl: "https://cdnjs.cloudflare.com/ajax/libs/leaflet/1.7.1/images/marker-shadow.png",
    });
  }, []);

  const getNodeColor = (node: Node) => {
    if (node.id === selectedStart) return "green";
    if (node.id === selectedEnd) return "red";
    if (currentStep?.visitedNodes.includes(node.id)) return "blue";
    if (currentStep?.frontier.includes(node.id)) return "orange";
    return node.type === 0 ? "gray" : "#6b7280";
  };

  const getEdgeStyle = (from: string, to: string) => {
    if (!pathResult?.path) return { color: "#6b7280", weight: 1, opacity: 0.5 };

    const isInPath = pathResult.path.some((nodeId, i) => {
      if (i === pathResult.path.length - 1) return false;
      return (
        (pathResult.path[i] === from && pathResult.path[i + 1] === to) ||
        (pathResult.path[i] === to && pathResult.path[i + 1] === from)
      );
    });

    if (isInPath) {
      return { color: "#3b82f6", weight: 3, opacity: 1 };
    }

    return { color: "#6b7280", weight: 1, opacity: 0.5 };
  };

  return (
    <MapContainer
      center={[31.7917, -7.0926]} // Center of Morocco
      zoom={6}
      className="h-[600px] w-full rounded-lg"
    >
      <TileLayer
        attribution='&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
        url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
      />

      {/* Draw edges */}
      {edges.map((edge, index) => {
        const fromNode = nodes.find((n) => n.id === edge.from);
        const toNode = nodes.find((n) => n.id === edge.to);
        if (!fromNode || !toNode) return null;

        return (
          <Polyline
            key={`${edge.from}-${edge.to}-${index}`}
            positions={[
              [fromNode.lat, fromNode.lng],
              [toNode.lat, toNode.lng],
            ]}
            pathOptions={getEdgeStyle(edge.from, edge.to)}
          />
        );
      })}

      {/* Draw nodes */}
      {nodes.map((node) => (
        <div key={node.id}>
          <Marker
            position={[node.lat, node.lng]}
            icon={L.divIcon({
              className: "airport-marker",
              html: `<div style="
                width: ${node.type === 0 ? '20px' : '12px'}; 
                height: ${node.type === 0 ? '20px' : '12px'}; 
                background-color: ${getNodeColor(node)}; 
                border-radius: 50%; 
                border: 2px solid white;
              "></div>`,
            })}
          >
            <Popup>
              <div className="text-sm">
                <h3 className="font-bold">{node.id}</h3>
                {'name' in node && <p>{node.name}</p>}
                {node.type === 0 && 'city' in node && (
                  <>
                    <p>{node.city}, {node.country}</p>
                    <p>Elevation: {node.elevation}ft</p>
                  </>
                )}
                {node.type === 1 && 'countryCode' in node && (
                  <p>{node.countryCode} - {node.countryName}</p>
                )}
                <p className="text-xs text-gray-500 mt-1">
                  Type: {node.type === 0 ? 'Airport' : 'Waypoint'}
                </p>
              </div>
            </Popup>
          </Marker>

          {/* Visualization circles for visited/frontier nodes */}
          {(currentStep?.visitedNodes.includes(node.id) ||
            currentStep?.frontier.includes(node.id)) && (
            <Circle
              center={[node.lat, node.lng]}
              radius={10000}
              pathOptions={{
                color: currentStep.visitedNodes.includes(node.id) ? "blue" : "orange",
                fillColor: currentStep.visitedNodes.includes(node.id) ? "blue" : "orange",
                fillOpacity: 0.2,
              }}
            />
          )}
        </div>
      ))}
    </MapContainer>
  );
}
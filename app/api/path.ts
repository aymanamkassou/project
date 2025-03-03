import type { VercelRequest, VercelResponse } from '@vercel/node';

export default async function handler(req: VercelRequest, res: VercelResponse) {
  if (req.method === 'POST') {
    try {
      const backendResponse = await fetch('http://35.180.209.123:3001/api/path', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify(req.body),
      });

      const data = await backendResponse.json();
      
      res.setHeader('Access-Control-Allow-Origin', '*');
      res.status(200).json(data);
    } catch (error) {
      console.error('Pathfinding error:', error);
      res.status(500).json({ error: 'Pathfinding failed' });
    }
  } else {
    res.status(405).json({ error: 'Method not allowed' });
  }
}
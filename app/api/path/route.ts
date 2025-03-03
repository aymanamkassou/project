export async function POST(request: Request) {
    try {
      const body = await request.json();
      
      const backendResponse = await fetch('http://35.180.209.123:3001/api/find-path', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(body),
      });
  
      if (!backendResponse.ok) {
        throw new Error(`Backend error: ${backendResponse.status}`);
      }
  
      const data = await backendResponse.json();
  
      return new Response(JSON.stringify(data), {
        status: 200,
        headers: {
          'Content-Type': 'application/json',
          'Access-Control-Allow-Origin': '*',
          'Access-Control-Allow-Methods': 'POST'
        }
      });
    } catch (error) {
      return new Response(JSON.stringify({ 
        error: error instanceof Error ? error.message : 'Failed to calculate path'
      }), {
        status: 500,
        headers: { 'Content-Type': 'application/json' }
      });
    }
  }